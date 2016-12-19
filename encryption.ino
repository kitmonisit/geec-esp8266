#include <sodium.h>
#include <errno.h>
#include "ESP8266TrueRandom.h" // true random number generator

static const char *
func_name(void)
{
    return "ESP8266TrueRandom";
}

static void
func_random_buf(void * const buf, const size_t size)
{
    ESP8266TrueRandom.memfill((char*) buf, size);
}

static void
bin2hex(unsigned char * const hex, const size_t hex_maxlen,
        const unsigned char * const bin, const size_t bin_len)
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

static int8_t
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

void
print_hex(const unsigned char *bin, const size_t bin_len)
{
    uint8_t       hex_size = bin_len*2 + 1;
    unsigned char hex[hex_size];

    bin2hex(hex, hex_size, bin, bin_len);
    Serial.printf("%s\n", hex);
}

void encrypt(
    unsigned char       *const nonce_ciphertext,
    const unsigned char *const nonce,
    const char          *const plaintext,
    const uint8_t       *const plaintext_len)
{
    unsigned char  client_sk[crypto_box_SECRETKEYBYTES];
    unsigned char  server_pk[crypto_box_PUBLICKEYBYTES];

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

    // Clear keys from memory
    sodium_memzero(nonce_ciphertext,
        crypto_box_NONCEBYTES
        + crypto_box_MACBYTES
        + *plaintext_len);
    sodium_memzero(client_sk, sizeof client_sk);
    sodium_memzero(server_pk, sizeof server_pk);

    // prepend nonce
    memcpy(nonce_ciphertext, nonce, crypto_box_NONCEBYTES);
    // client_sk: Hex decode CLIENT_SK_HEX
    hex2bin(client_sk, crypto_box_SECRETKEYBYTES,
            CLIENT_SK_HEX, crypto_box_SECRETKEYBYTES*2,
            NULL, NULL, NULL);
    // server_pk: Hex decode SERVER_PK_HEX
    hex2bin(server_pk, crypto_box_PUBLICKEYBYTES,
            SERVER_PK_HEX, crypto_box_PUBLICKEYBYTES*2,
            NULL, NULL, NULL);

    // Encrypt plaintext and append to nonce
    crypto_box_primitive();
    crypto_box_easy(nonce_ciphertext+crypto_box_NONCEBYTES, (const unsigned char *)plaintext, *plaintext_len, nonce_ciphertext, server_pk, client_sk);

    // Clear keys from memory
    sodium_memzero(client_sk, sizeof client_sk);
    sodium_memzero(server_pk, sizeof server_pk);
}

void sign(
    unsigned char            *const signedtext_hex,
    const unsigned char      *const nonce_ciphertext,
    const unsigned long long *const nonce_ciphertext_len)
{
    unsigned char       client_ssk[crypto_sign_SECRETKEYBYTES];
    unsigned char       signedtext[crypto_sign_BYTES + *nonce_ciphertext_len];
    unsigned long long  signedtext_len = 0;
    uint16_t            signedtext_hex_len;

    Serial.print(F("nonce + ciphertext is\n  "));
    print_hex(nonce_ciphertext, *nonce_ciphertext_len);

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

    // Clear keys from memory
    sodium_memzero(client_ssk, sizeof client_ssk);

    // client_ssk: Hex decode CLIENT_SSK_HEX
    hex2bin(client_ssk, crypto_sign_SECRETKEYBYTES,
            CLIENT_SSK_HEX, crypto_sign_SECRETKEYBYTES*2,
            NULL, NULL, NULL);
    // Sign nonce_ciphertext and write into signedtext
    crypto_sign_primitive();
    crypto_sign(signedtext, &signedtext_len, nonce_ciphertext, *nonce_ciphertext_len, client_ssk);

    // Clear keys from memory
    sodium_memzero(client_ssk, sizeof client_ssk);

    // Encode signedtext and write into signedtext_hex
    signedtext_hex_len = signedtext_len*2 + 1;
    bin2hex(signedtext_hex, signedtext_hex_len, signedtext, signedtext_len);
}

// vim:fdm=syntax

