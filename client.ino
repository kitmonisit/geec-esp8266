#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <sodium.h>

#define HOST           "vast-lake-95491.herokuapp.com"
#define HTTP_HOST      "http://vast-lake-95491.herokuapp.com"
#define HTTP_PORT      80
// #define HOST           "192.168.22.4"
// #define HTTP_HOST      "http://192.168.22.4:5000"
// #define HTTP_PORT   5000
#define CLIENT_NAME    "node04_"
#define CLIENT_SK_HEX  "f0a6bd567547b1f2905b0bc0d7db4d903084d6d3883616ff1086f3b219743a14"
#define CLIENT_SSK_HEX "cb89b7d0a4d65ed8a8207220035f63b74352e0203a275859f577ce3db33d563d8e4ff2eb2a744b71f5e4f6f389fbcecea33966a765a5c13a622f109b78dabdec"
#define SERVER_PK_HEX  "5f8331082dc3f70428ac739a1a7981f911d7f0d3c0e0e583ad7f35c00faa141e"
#define TIMEOUT        (uint16_t) 10000

static WiFiClient client;
static HTTPClient http;

static uint16_t      httpCode;
static String        cookie;
static unsigned char nonce[crypto_box_NONCEBYTES];
static uint8_t       stream_msg = 0;

void print_hex(const unsigned char *bin, const size_t bin_len);

void stream_updates_to_cloud(void)
{
    if (!client.connect(HOST, HTTP_PORT)) {
        Serial.println(F("connection failed"));
    }

    String url = "/stream";
    String payload = String("POST ") + url + " HTTP/1.1\r\n"
            + "Host: " + HOST + "\r\n"
            + "Connection: keep-alive\r\n"
            + "Transfer-Encoding: chunked\r\n\r\n";
    Serial.println();
    client.print(payload);
    client.print("6\r\n");
    client.print("hello \r\n");
    client.print("6\r\n");
    client.print("world \r\n");
    client.print("7\r\n");
    client.print("chunked\r\n");
    client.print("0\r\n");
    client.print("\r\n");
    Serial.print(payload);

    delay(2000);
    Serial.print(F("response\n"));
    int buf_size = client.available();
    uint8_t buf[buf_size];
    client.read(buf, buf_size);
    int idx = 0;
    while(idx < buf_size) {
        Serial.printf("%c", *(buf + idx));
        idx++;
    }
}

void send_updates_to_cloud(const char *const plaintext)
{
    // Message length is limited to 512 characters

    uint16_t plaintext_len = 0;
    while (*(plaintext+plaintext_len) != '\0') {
        plaintext_len++;
    }

    digitalWrite(LED_BLUE, LOW);
    request_nonce();
    if (httpCode == HTTP_CODE_OK) send_message(plaintext, &plaintext_len);
    digitalWrite(LED_BLUE, HIGH);
}

static void request_nonce(void)
{
    const char *headerkeys[] = {"Set-Cookie"};
    const char  headerkeyssize = sizeof(headerkeys) / sizeof(char*);

    http.setTimeout(TIMEOUT);
    Serial.print(F("[HTTP] begin ...\n"));
    http.begin(String(HTTP_HOST) + "/nonce");

    // Before executing GET, specify which headers to collect
    http.collectHeaders(headerkeys, headerkeyssize);

    Serial.print(F("[HTTP] GET /nonce\n"));
    httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK) {
        process_cookie(http.header("Set-Cookie"));
        memcpy(nonce, http.getString().c_str(), crypto_box_NONCEBYTES);
        Serial.print(F("[HTTP] GET /nonce successful\n"));
        Serial.print(F("cookie is\n  "));
        Serial.println(cookie);
        Serial.print(F("nonce is\n  "));
        print_hex(nonce, crypto_box_NONCEBYTES);
    } else {
        Serial.print(F("[HTTP] GET failed, error: "));
        Serial.printf("%s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
    Serial.print(F("[HTTP] end ...\n"));
}

static void *process_cookie(String pre_cookie)
{
    uint16_t len_cookie = (uint8_t) pre_cookie.indexOf(';', 0);
    cookie = pre_cookie.substring(0, len_cookie);
    // pre_cookie.toCharArray(cookie, len_cookie);
}

static void send_message(
    const char     *const plaintext,
    const uint16_t *const plaintext_len)
{
    unsigned long long nonce_ciphertext_len = crypto_box_NONCEBYTES + crypto_box_MACBYTES + *plaintext_len;
    unsigned char      nonce_ciphertext[nonce_ciphertext_len];
    unsigned char      signedtext_hex[(crypto_sign_BYTES + nonce_ciphertext_len)*2 + 1];

    uint16_t idx = 0;
    String payload = String(CLIENT_NAME);
    String response;

    http.setReuse(false);
    Serial.print(F("[HTTP] begin ...\n"));
    http.begin(String(HTTP_HOST) + "/send_message");
    http.addHeader(F("Cookie"), cookie);
    // DONE: uint8_t nonce_bytes: Hex decode nonce_hex (24 bytes)
    // DONE: uint8_t server_pk_bytes: Hex decode SERVER_PK_HEX
    // DONE: uint8_t client_sk_bytes: Hex decode CLIENT_SK_HEX
    // DONE: uint8_t client_ssk_bytes: Hex decode CLIENT_SSK_HEX
    // DONE: uint8_t ciphertext_bytes: Encrypt (plaintext, nonce_bytes, server_pk_bytes, client_sk_bytes)
    // DONE: uint8_t signedtext: Sign (client_ssk_bytes, nonce_bytes + ciphertext_bytes)
    // DONE: uint8_t signedtext_hex: Hex encode (signedtext)
    // DONE: uint8_t full_message: client_signedtext_hex

    Serial.print(F("plaintext is\n  "));
    Serial.printf("%s\n", plaintext);
    // Encrypt
    encrypt(nonce_ciphertext, nonce, plaintext, plaintext_len);
    // Sign
    sign(signedtext_hex, nonce_ciphertext, &nonce_ciphertext_len);
    Serial.print(F("signedtext is\n  "));
    Serial.printf("%s\n", signedtext_hex);

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
        Serial.print(F("response from server is\n  "));
        Serial.println(response);
    } else {
        Serial.print(F("[HTTP] POST failed, error: "));
        Serial.printf("%s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
    Serial.print(F("[HTTP] end ...\n"));
}

void stream_begin(void)
{
    // Need this for cookie
    request_nonce();

    if (!client.connect(HOST, HTTP_PORT)) {
        Serial.println(F("connection failed"));
    }

    String url = "/send_message";
    String payload = String("POST ") + url + " HTTP/1.1\r\n"
            + "Host: " + HOST + "\r\n"
            + "Connection: keep-alive\r\n"
            + "Cookie: " + cookie + "\r\n"
            + "Transfer-Encoding: chunked\r\n\r\n";
    client.print(payload);
    Serial.println(F("stream_begin"));
}

void stream_add(
    const char *const plaintext)
{
    String payload = String(CLIENT_NAME);

    char payload_len[7];
    uint16_t idx = 0;
    uint16_t plaintext_len = 0;

    while (*(plaintext+plaintext_len) != '\0') {
        plaintext_len++;
    }

    unsigned long long nonce_ciphertext_len =
        crypto_box_NONCEBYTES
        + crypto_box_MACBYTES
        + plaintext_len;
    unsigned char      nonce_ciphertext[nonce_ciphertext_len];
    unsigned char      signedtext_hex[(crypto_sign_BYTES + nonce_ciphertext_len)*2 + 1];

    Serial.print(F("plaintext is\n  "));
    Serial.printf("%s\n", plaintext);
    // Encrypt
    // TODO Add stream_msg to nonce
    encrypt(nonce_ciphertext, nonce, plaintext, &plaintext_len);
    // Sign
    sign(signedtext_hex, nonce_ciphertext, &nonce_ciphertext_len);
    Serial.print(F("signedtext is\n  "));
    Serial.printf("%s\n", signedtext_hex);

    idx = 0;
    if (stream_msg > 0) {
        payload = "";
        idx = (crypto_sign_BYTES + crypto_box_NONCEBYTES)*2;
    }
    while (*(signedtext_hex + idx) != '\0') {
        payload.concat((char) *(signedtext_hex + idx));
        idx++;
    }
    sprintf(payload_len, "%0X\r\n", payload.length()+1);

    client.print(payload_len);
    payload.concat("\n\r\n");
    client.print(payload);
    Serial.print(payload);
    stream_msg++;
}

void stream_end(
    void)
{
    client.print("0\r\n");
    client.print("\r\n");
    stream_msg = 0;
    Serial.print(F("stream_end\n"));

    Serial.print(F("\nresponse\n"));
    delay(5000);
    int buf_size = client.available();
    uint8_t buf[buf_size];
    client.read(buf, buf_size);
    Serial.println();

    uint16_t idx = 0;
    while(idx < buf_size) {
        Serial.printf("%c", *(buf + idx));
        idx++;
    }
}


// vim:fdm=syntax

