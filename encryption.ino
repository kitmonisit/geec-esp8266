#include <sodium.h>

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
        /*Serial.printf("x is %x\n", x);*/
        /*Serial.printf("i is %d\n", i);*/
        hex[i * 2] = (char) x;
        x >>= 8;
        hex[i * 2 + 1] = (char) x;
        i++;
    }
    hex[i * 2U] = 0U;
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

int box(void)
{
    unsigned char bob_pk[crypto_box_PUBLICKEYBYTES]; /* Bob's public key */
    unsigned char bob_sk[crypto_box_SECRETKEYBYTES]; /* Bob's secret key */

    unsigned char alice_pk[crypto_box_PUBLICKEYBYTES]; /* Alice's public key */
    unsigned char alice_sk[crypto_box_SECRETKEYBYTES]; /* Alice's secret key */

    unsigned char nonce[crypto_box_NONCEBYTES] = {
        0x41,0x61,0x41,0x61,0x41,0x61,
        0x41,0x61,0x41,0x61,0x41,0x61,
        0x41,0x61,0x41,0x61,0x41,0x61,
        0x41,0x61,0x41,0x61,0x41,0x61
    };
    unsigned char message[] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
    unsigned char ciphertext[crypto_box_MACBYTES + sizeof(message)];
    size_t        message_len;
    size_t        ciphertext_len;
    int           ret;

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

    crypto_box_keypair(bob_pk, bob_sk);     // generate Bob's keys
    crypto_box_keypair(alice_pk, alice_sk); // generate Alice's keys
    randombytes_buf(nonce, sizeof(nonce));
    Serial.print(F("\nbob_pk: "));
    print_hex(bob_pk, sizeof(bob_pk));
    Serial.print(F("bob_sk: "));
    print_hex(bob_sk, sizeof(bob_sk));
    Serial.print(F("alice_pk: "));
    print_hex(alice_pk, sizeof(alice_pk));
    Serial.print(F("alice_sk: "));
    print_hex(alice_sk, sizeof(alice_sk));
    Serial.print(F("\nnonce: "));
    print_hex(nonce, crypto_box_NONCEBYTES);

    message_len = sizeof(message);

    crypto_box_primitive();
    // Encrypt message and write into ciphertext
    crypto_box_easy(ciphertext, message, message_len, nonce, alice_pk, bob_sk);
    Serial.print(F("ciphertext: "));
    print_hex(ciphertext, crypto_box_MACBYTES + message_len);
    ciphertext_len = crypto_box_MACBYTES + message_len;

    // Decrypt ciphertext and write into message
    ret = crypto_box_open_easy(message, ciphertext, ciphertext_len, nonce, bob_pk, alice_sk);
    Serial.print(F("message: "));
    print_hex(message, message_len);
    Serial.print(F("plaintext: "));
    Serial.printf("%s\n", message);

    sodium_memzero(bob_sk, sizeof bob_sk); // wipe sensitive data
    sodium_memzero(alice_sk, sizeof alice_sk);
    sodium_memzero(message, sizeof message);
    sodium_memzero(ciphertext, sizeof ciphertext);

    return ret;
}

// vim:fdm=syntax

