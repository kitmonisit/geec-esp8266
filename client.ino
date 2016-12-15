#include <ESP8266HTTPClient.h>

#define HOST "http://tranquil-dusk-55557.herokuapp.com"
#define CLIENT_SK_HEX "2c01132c8bcf2ffa1f97c12c414416830fbbb29c5fea0c7447ac92c3186991e3"
#define SERVER_PK_HEX "06ccf32ab75cddb9ff0b9ba7ecaf1aa401c6d76fcac9fdb10b447f8fdb7b790c"

static HTTPClient  http;

static int  httpCode;
static char cookie[112];
static char nonce_hex[crypto_box_NONCEBYTES*2 + 1];

void send_updates_to_cloud(char * const message) {
    int idx = 0;
    int message_len = 0;
    while (*(message+idx) != '\0') {
        message_len++;
        idx++;
    }

    request_nonce();
    if (httpCode == HTTP_CODE_OK) send_message(cookie, message, &message_len);
}

static void request_nonce(void) {
    const char *headerkeys[] = {"Set-Cookie"};
    const char  headerkeyssize = sizeof(headerkeys) / sizeof(char*);

    Serial.print(F("[HTTP] begin ...\n"));

    http.begin(String(HOST) + "/request_nonce");
    // Before executing GET, specify which headers to collect
    http.collectHeaders(headerkeys, headerkeyssize);

    Serial.print(F("[HTTP] GET /request_nonce\n"));
    httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK) {
        process_cookie(http.header("Set-Cookie"), cookie);
        strcpy(nonce_hex, http.getString().c_str());
        http.end();
    } else {
        Serial.printf("[HTTP] GET failed, error: %s\n", http.errorToString(httpCode).c_str());
        http.end();
    }
    Serial.print(F("[HTTP] end ...\n"));
    Serial.printf("nonce\n%s\n", nonce_hex);
}

static void *process_cookie(String pre_cookie, char * const cookie) {
    const char *scan_cookie = pre_cookie.c_str();

    // Cookie is the portion of the value of Set-Cookie header before the ';'
    // Count the number of characters before the ';'
    char len_cookie = 0; // cookie is less than 256 bytes long
    while(*scan_cookie != ';') {
        scan_cookie++;
        len_cookie++;
    }

    // Store the characters before the ';' into cookie
    strcpy(cookie, pre_cookie.substring(0, len_cookie).c_str());
    Serial.println("cookie is");
    Serial.println(pre_cookie);
    int idx = 0;
    while (*(cookie + idx) != '\0') {
        Serial.printf("%c", *(cookie + idx));
        idx++;
    }
    Serial.println();
}

static void send_message(char * const cookie, char * const message, int * const message_len) {
    unsigned char ciphertext_hex[(crypto_box_MACBYTES + *message_len)*2 + 1];
    int idx = 0;
    String payload = String("client_");
    String cookie_str;
    String response;

    idx = 0;
    while (*(cookie + idx) != '\0') {
        cookie_str.concat((char) *(cookie + idx));
        idx++;
    }
    http.begin(String(HOST) + "/send_message");
    http.addHeader("Cookie", cookie_str);
    // TODO: uint8_t nonce_bytes: Hex decode nonce_hex 192 bits (24 bytes)
    // TODO: uint8_t server_pk_bytes: Hex decode SERVER_PK_HEX
    // TODO: uint8_t client_sk_bytes: Hex decode CLIENT_SK_HEX
    // TODO: uint8_t client_ssk_bytes: Hex decode CLIENT_SSK_HEX
    // TODO: uint8_t ciphertext_bytes: Encrypt (nonce_bytes, server_pk_bytes, client_sk_bytes, message)
    // TODO: uint8_t ciphertext_hex: Hex encode (ciphertext)
    // TODO: uint8_t signedtext: Sign (client_ssk_bytes, ciphertext_hex)
    // TODO: uint8_t signedtext_hex: Hex encode (signedtext)
    // TODO: uint8_t full_message: client_signedtext
    encrypt(ciphertext_hex, nonce_hex, message, message_len);
    Serial.println("ciphertext is");
    idx = 0;
    while (*(ciphertext_hex + idx) != '\0') {
        Serial.printf("%c", *(ciphertext_hex + idx));
        idx++;
    }
    Serial.println();
    idx = 0;
    while (idx < crypto_box_NONCEBYTES*2) {
        payload.concat((char) *(nonce_hex + idx));
        idx++;
    }
    idx = 0;
    while (*(ciphertext_hex + idx) != '\0') {
        payload.concat((char) *(ciphertext_hex + idx));
        idx++;
    }
    /*plaintext = sender + "_" + nonce_hex + message;*/

    Serial.println("[HTTP] POST /send_message");
    Serial.println(payload);
    httpCode = http.POST(payload);
    if (httpCode == HTTP_CODE_OK) {
        response = http.getString();
        Serial.println(response);
    } else {
        Serial.printf("[HTTP] POST failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
}

// vim:fdm=syntax

