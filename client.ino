#include <ESP8266HTTPClient.h>
#include <sodium.h>

#define HOST           "http://vast-lake-95491.herokuapp.com"
#define CLIENT_NAME    "node04_"
#define CLIENT_SK_HEX  "f0a6bd567547b1f2905b0bc0d7db4d903084d6d3883616ff1086f3b219743a14"
#define CLIENT_SSK_HEX "cb89b7d0a4d65ed8a8207220035f63b74352e0203a275859f577ce3db33d563d8e4ff2eb2a744b71f5e4f6f389fbcecea33966a765a5c13a622f109b78dabdec"
#define SERVER_PK_HEX  "5f8331082dc3f70428ac739a1a7981f911d7f0d3c0e0e583ad7f35c00faa141e"

static HTTPClient http;

static uint16_t httpCode;
static char     cookie[156];
static char     nonce_hex[crypto_box_NONCEBYTES*2 + 1];

void send_updates_to_cloud(const char *const plaintext)
{
    // Message length is limited to 512 characters

    uint16_t plaintext_len = 0;
    while (*(plaintext+plaintext_len) != '\0') {
        plaintext_len++;
    }

    request_nonce();
    if (httpCode == HTTP_CODE_OK) send_message(cookie, plaintext, &plaintext_len);
}

static void request_nonce(void)
{
    const char *headerkeys[] = {"Set-Cookie"};
    const char  headerkeyssize = sizeof(headerkeys) / sizeof(char*);

    Serial.print(F("[HTTP] begin ...\n"));
    http.begin(String(HOST) + "/nonce");

    // Before executing GET, specify which headers to collect
    http.collectHeaders(headerkeys, headerkeyssize);

    Serial.print(F("[HTTP] GET /nonce\n"));
    httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK) {
        process_cookie(http.header("Set-Cookie"));
        strcpy(nonce_hex, http.getString().c_str());
        Serial.print(F("[HTTP] GET /nonce successful\n"));
        Serial.printf("cookie is\n  %s\n", cookie);
        Serial.printf("nonce is\n  %s\n", nonce_hex);
    } else {
        Serial.printf("[HTTP] GET failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
    Serial.print(F("[HTTP] end ...\n"));
}

static void *process_cookie(String pre_cookie)
{
    uint8_t len_cookie = (uint8_t) pre_cookie.indexOf(';') + 1;
    pre_cookie.toCharArray(cookie, len_cookie);
}

static void send_message(
    const char     *const cookie,
    const char     *const plaintext,
    const uint16_t *const plaintext_len)
{
    unsigned long long nonce_ciphertext_len = crypto_box_NONCEBYTES + crypto_box_MACBYTES + *plaintext_len;
    unsigned char      nonce_ciphertext[nonce_ciphertext_len];
    unsigned char      signedtext_hex[(crypto_sign_BYTES + nonce_ciphertext_len)*2 + 1];

    uint16_t idx = 0;
    String payload = String(CLIENT_NAME);
    String cookie_str;
    String response;

    idx = 0;
    while (*(cookie + idx) != '\0') {
        cookie_str.concat((char) *(cookie + idx));
        idx++;
    }

    Serial.print(F("[HTTP] begin ...\n"));
    http.begin(String(HOST) + "/send_message");
    http.addHeader("Cookie", cookie_str);
    // DONE: uint8_t nonce_bytes: Hex decode nonce_hex (24 bytes)
    // DONE: uint8_t server_pk_bytes: Hex decode SERVER_PK_HEX
    // DONE: uint8_t client_sk_bytes: Hex decode CLIENT_SK_HEX
    // DONE: uint8_t client_ssk_bytes: Hex decode CLIENT_SSK_HEX
    // DONE: uint8_t ciphertext_bytes: Encrypt (plaintext, nonce_bytes, server_pk_bytes, client_sk_bytes)
    // DONE: uint8_t signedtext: Sign (client_ssk_bytes, nonce_bytes + ciphertext_bytes)
    // DONE: uint8_t signedtext_hex: Hex encode (signedtext)
    // DONE: uint8_t full_message: client_signedtext_hex

    Serial.printf("plaintext is\n  %s\n", plaintext);
    // Encrypt
    encrypt(nonce_ciphertext, nonce_hex, plaintext, plaintext_len);
    // Sign
    sign(signedtext_hex, nonce_ciphertext, &nonce_ciphertext_len);
    Serial.printf("signedtext is\n  %s\n", signedtext_hex);

    idx = 0;
    while (*(signedtext_hex + idx) != '\0') {
        payload.concat((char) *(signedtext_hex + idx));
        idx++;
    }

    Serial.println(F("[HTTP] POST /send_message"));
    Serial.println(payload);
    httpCode = http.POST(payload);
    if (httpCode == HTTP_CODE_OK) {
        response = http.getString();
        Serial.print(F("[HTTP] POST /send_message successful\n"));
        Serial.printf("response from server is\n  %s\n", response.c_str());
    } else {
        Serial.printf("[HTTP] POST failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
    Serial.print(F("[HTTP] end ...\n"));
}

// vim:fdm=syntax

