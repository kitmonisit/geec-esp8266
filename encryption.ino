#include <sodium.h>
#include <string.h>
#include <errno.h>

#define MAX_INPUT_LEN 512

static const char *func_name(void) {
    return "dummy";
}

static void func_random_buf(void * const buf, const size_t size) {
    ESP8266TrueRandom.memfill(buf, size);
}

static void *bin2hex(char * const hex, const size_t hex_maxlen, const unsigned char * const bin, const size_t bin_len)
{
    // `hex` must be a pointer to an array of size `hex_maxlen`
    // Result will be written to `hex`

    int          i = 0;
    unsigned int x;
    int          b;
    int          c;

    /*if (bin_len >= SIZE_MAX / 2 || hex_maxlen <= bin_len * 2) {*/
        /*return NULL;*/
    /*}*/
    while (i < bin_len) {
        c = bin[i] & 0xf;
        b = bin[i] >> 4;
        x = (unsigned char) (87 + c + (((c - 10) >> 8) & ~38)) << 8 |
            (unsigned char) (87 + b + (((b - 10) >> 8) & ~38));
        hex[i * 2] = (char) x;
        x >>= 8;
        hex[i * 2 + 1] = (char) x;
        i++;
    }
    hex[i * 2U] = 0U;
}

int
hex2bin(unsigned char * const bin, const size_t bin_maxlen,
               const char * const hex, const size_t hex_len,
               const char * const ignore, int * const bin_len,
               const char ** const hex_end)
{
    size_t        bin_pos = (size_t) 0U;
    size_t        hex_pos = (size_t) 0U;
    int           ret = 0;
    unsigned char c;
    unsigned char c_acc = 0U;
    unsigned char c_alpha0, c_alpha;
    unsigned char c_num0, c_num;
    unsigned char c_val;
    unsigned char state = 0U;

    while (hex_pos < hex_len) {
        c = (unsigned char) hex[hex_pos];
        c_num = c ^ 48U;
        c_num0 = (c_num - 10U) >> 8;
        c_alpha = (c & ~32U) - 55U;
        c_alpha0 = ((c_alpha - 10U) ^ (c_alpha - 16U)) >> 8;
        if ((c_num0 | c_alpha0) == 0U) {
            if (ignore != NULL && state == 0U && strchr(ignore, c) != NULL) {
                hex_pos++;
                continue;
            }
            break;
        }
        c_val = (c_num0 & c_num) | (c_alpha0 & c_alpha);
        if (bin_pos >= bin_maxlen) {
            ret = -1;
            errno = ERANGE;
            break;
        }
        if (state == 0U) {
            c_acc = c_val * 16U;
        } else {
            bin[bin_pos++] = c_acc | c_val;
        }
        state = ~state;
        hex_pos++;
    }
    if (state != 0U) {
        hex_pos--;
    }
    if (hex_end != NULL) {
        *hex_end = &hex[hex_pos];
    }
    if (bin_len != NULL) {
        *bin_len = bin_pos;
    }
    return ret;
}

void print_hex(const unsigned char *bin, const size_t bin_len) {
    int  hex_size = bin_len*2 + 1;
    char hex[hex_size];

    // This next line fixed the exception problem
    // WEIRD WEIRD WEIRD
    /*Serial.printf("\nfirst hex is %p\n", &hex);*/

    bin2hex(hex, hex_size, bin, bin_len);
    Serial.printf("%s\n", hex);
}

void encrypt(char * const ciphertext_hex, char * const nonce_hex, char * const message, int * const message_len) {
    unsigned char  client_sk[crypto_box_SECRETKEYBYTES];
    unsigned char  server_pk[crypto_box_PUBLICKEYBYTES];
    unsigned char  ciphertext[crypto_box_MACBYTES + *message_len];
    unsigned char  nonce[crypto_box_NONCEBYTES];
    int            nonce_hex_len = crypto_box_NONCEBYTES*2;
    int            ciphertext_len;
    int            ciphertext_hex_len;
    int            bin_len;
    const char    *hex_end;

    struct randombytes_implementation impl = {
        SODIUM_C99(.implementation_name =) func_name,
        SODIUM_C99(.random =) NULL,
        SODIUM_C99(.stir =) NULL,
        SODIUM_C99(.uniform =) NULL,
        SODIUM_C99(.buf =) func_random_buf,
        SODIUM_C99(.close =) NULL,
    };
    randombytes_set_implementation(&impl);
    sodium_init();

    sodium_memzero(client_sk, sizeof client_sk);
    sodium_memzero(server_pk, sizeof server_pk);

    hex2bin(nonce, sizeof(nonce), nonce_hex, nonce_hex_len, NULL, &bin_len, &hex_end);
    hex2bin(client_sk, sizeof(client_sk), CLIENT_SK_HEX, strlen(CLIENT_SK_HEX), NULL, &bin_len, &hex_end);
    hex2bin(server_pk, sizeof(client_sk), SERVER_PK_HEX, strlen(CLIENT_SK_HEX), NULL, &bin_len, &hex_end);

    crypto_box_primitive();
    // Encrypt message and write into ciphertext
    crypto_box_easy(ciphertext, message, *message_len, nonce, server_pk, client_sk);
    ciphertext_len = sizeof(ciphertext);
    ciphertext_hex_len = ciphertext_len*2 + 1;
    bin2hex(ciphertext_hex, ciphertext_hex_len, ciphertext, ciphertext_len);
}

// vim:fdm=syntax

