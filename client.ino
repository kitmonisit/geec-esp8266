#include <ESP8266HTTPClient.h>

#define HOST "http://tranquil-dusk-55557.herokuapp.com"
#define CLIENT_SK_HEX "2c01132c8bcf2ffa1f97c12c414416830fbbb29c5fea0c7447ac92c3186991e3"
#define SERVER_PK_HEX "06ccf32ab75cddb9ff0b9ba7ecaf1aa401c6d76fcac9fdb10b447f8fdb7b790c"

static HTTPClient http;

void send_updates_to_cloud(String message) {
    String      cookie;
    String      nonce_hex;
    int         httpCode;
    const char *headerkeys[] = {"Set-Cookie"};
    const char  headerkeyssize = sizeof(headerkeys) / sizeof(char*);

    Serial.println("[HTTP] begin ...");

    httpCode = http.begin(String(HOST) + "/request_nonce");
    // Before executing GET, specify which headers to collect
    http.collectHeaders(headerkeys, headerkeyssize);

    Serial.println("[HTTP] GET /request_nonce");
    http.GET();
    if (httpCode = HTTP_CODE_OK) {
        cookie = process_cookie(http.header("Set-Cookie"));
        nonce_hex = http.getString();
        http.end();
        send_message(cookie, nonce_hex, message);
    } else {
        Serial.printf("[HTTP] GET failed, error: %s\n", http.errorToString(httpCode).c_str());
        http.end();
    }
    Serial.println("[HTTP] end ...");
}

static String process_cookie(String pre_cookie) {
    const char *scan_cookie = pre_cookie.c_str();

    // Cookie is the portion of the value of Set-Cookie header before the ';'
    // Count the number of characters before the ';'
    char len_cookie = 0; // cookie is less than 256 bytes long
    while(*scan_cookie != ';') {
        scan_cookie++;
        len_cookie++;
    }

    // Store the characters before the ';' into cookie
    String cookie = pre_cookie.substring(0, len_cookie);
    return cookie;
}

static void send_message(String cookie, String nonce_hex, String message) {
    String sender = "client";
    String plaintext;
    String response;
    int httpCode;

    http.begin(String(HOST) + "/send_message");
    http.addHeader("Cookie", cookie);
    // TODO: uint8_t nonce_bytes: Hex decode nonce_hex 192 bits (24 bytes)
    // TODO: uint8_t server_pk_bytes: Hex decode SERVER_PK_HEX
    // TODO: uint8_t client_sk_bytes: Hex decode CLIENT_SK_HEX
    // TODO: uint8_t client_ssk_bytes: Hex decode CLIENT_SSK_HEX
    // TODO: uint8_t ciphertext_bytes: Encrypt (nonce_bytes, server_pk_bytes, client_sk_bytes, message)
    // TODO: uint8_t ciphertext_hex: Hex encode (ciphertext)
    // TODO: uint8_t signedtext: Sign (client_ssk_bytes, ciphertext_hex)
    // TODO: uint8_t signedtext_hex: Hex encode (signedtext)
    // TODO: uint8_t full_message: client_signedtext
    plaintext = sender + "_" + nonce_hex + message;

    Serial.println("[HTTP] POST /send_message");
    Serial.println(plaintext);
    httpCode = http.POST(plaintext);
    if (httpCode = HTTP_CODE_OK) {
        response = http.getString();
        Serial.println(response);
    } else {
        Serial.printf("[HTTP] POST failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
}

// vim:fdm=syntax

