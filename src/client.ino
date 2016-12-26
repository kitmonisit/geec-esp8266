#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <sodium.h>

#define HOST           "vast-lake-95491.herokuapp.com"
#define HTTP_HOST      "http://vast-lake-95491.herokuapp.com"
#define HTTP_PORT      80
// #define HOST           "192.168.22.4"
// #define HTTP_HOST      "http://192.168.22.4:5000"
// #define HTTP_PORT      5000
#define CLIENT_NAME    "node04"
#define CLIENT_SK_HEX  "f0a6bd567547b1f2905b0bc0d7db4d903084d6d3883616ff1086f3b219743a14"
#define CLIENT_SSK_HEX "cb89b7d0a4d65ed8a8207220035f63b74352e0203a275859f577ce3db33d563d8e4ff2eb2a744b71f5e4f6f389fbcecea33966a765a5c13a622f109b78dabdec"
#define SERVER_PK_HEX  "5f8331082dc3f70428ac739a1a7981f911d7f0d3c0e0e583ad7f35c00faa141e"
#define TIMEOUT        (uint16_t) 30000
#define MAX_PLAINTEXT_LEN 130

static WiFiClient client;

static char          cookie[198];
static unsigned char nonce[crypto_box_NONCEBYTES];
static uint8_t       stream_msg = 0;

static void func_random_buf(void * const buf, const size_t size);
static void bin2hex(unsigned char * const hex, const size_t hex_maxlen,
        const unsigned char * const bin, const size_t bin_len);
static int8_t hex2bin(unsigned char * const bin, const size_t bin_maxlen,
        const char * const hex, const size_t hex_len,
        const char * const ignore, int * const bin_len,
        const char ** const hex_end);
static void print_hex(const unsigned char *bin, const size_t bin_len);

static void ensure_WiFi(void)
{
    if (WiFi.status() != WL_CONNECTED) {
        connectToWiFi();
    } else {
        return;
    }
}

static void process_cookie(const char *const pre_cookie)
{
    uint16_t len_cookie = 0;
    while (*(pre_cookie + len_cookie) != ';') {
        len_cookie++;
    }
    strncpy(cookie, pre_cookie, len_cookie);
    *(cookie + len_cookie) = '\0';
}

static void request_nonce(void)
{
    HTTPClient  http;
    uint16_t    httpCode;
    const char *headerkeys[] = {"Set-Cookie"};
    const char  headerkeyssize = sizeof(headerkeys) / sizeof(char*);

    http.setTimeout(TIMEOUT);
    http.begin(String(HTTP_HOST) + "/nonce");

    // Before executing GET, specify which headers to collect
    http.collectHeaders(headerkeys, headerkeyssize);

    while (http.GET() != HTTP_CODE_OK);
    httpCode = HTTP_CODE_OK;

    if (httpCode == HTTP_CODE_OK) {
        process_cookie(http.header("Set-Cookie").c_str());
        hex2bin(
            nonce, crypto_box_NONCEBYTES,
            http.getString().c_str(), crypto_box_NONCEBYTES*2 + 1,
            NULL, NULL, NULL);
    } else {
        Serial.print(F("[HTTP] GET failed, error: "));
        Serial.printf("%s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
}

static void generate_own_nonce()
{
    if (stream_msg > 0) {
        func_random_buf(nonce, crypto_box_NONCEBYTES);
    }
}

void stream_begin(void)
{
    Serial.println(F("\n\nstream_begin"));
    digitalWrite(LED_BLUE, LOW);
    ensure_WiFi();

    // Need this for cookie
    request_nonce();

    String payload = String("POST ") + "/send_message" + " HTTP/1.1\r\n"
            + "Host: " + HOST + "\r\n"
            + "Connection: close\r\n"
            + "Cookie: " + cookie + "\r\n"
            + "Transfer-Encoding: chunked\r\n\r\n";

    while (!client.connected()) {
        client.connect(HOST, HTTP_PORT);
    }
    client.print(payload);
}

void stream_add(
    const char *const input)
{
    // input must be limited to 128 bytes total including null terminator
    ensure_WiFi();
    String payload;

    char payload_len[7];
    uint16_t idx = 0;
    const uint8_t plaintext_len = MAX_PLAINTEXT_LEN;
    char plaintext[plaintext_len] = {0};

    // Prepend orig input length
    sprintf(plaintext, "%02X", (uint8_t) strlen(input));
    // Append input string
    strcpy(plaintext+2, input);

    unsigned long long nonce_ciphertext_len =
        crypto_box_NONCEBYTES
        + crypto_box_MACBYTES
        + plaintext_len;
    unsigned char      nonce_ciphertext[nonce_ciphertext_len];
    unsigned char      signedtext_hex[(crypto_sign_BYTES + nonce_ciphertext_len)*2 + 1];

    // Encrypt
    generate_own_nonce();
    encrypt(nonce_ciphertext, nonce, plaintext, &plaintext_len);
    if (stream_msg != 0) {
        if (stream_msg == 1) {
            // Zero out the signature
            memset(signedtext_hex, 0, crypto_sign_BYTES*2);
        }
        // Write the hex encoded value of the nonce_ciphertext into its designated portion
        bin2hex(
            signedtext_hex + (crypto_sign_BYTES)*2, nonce_ciphertext_len*2+1,
            nonce_ciphertext, nonce_ciphertext_len);
    }

    // Sign
    if (stream_msg == 0) {
        // Sign only the first chunk
        sign(signedtext_hex, nonce_ciphertext, &nonce_ciphertext_len);
    }

    // Compose chunked data
    // https://en.wikipedia.org/wiki/Chunked_transfer_encoding#Example
    // payload: client_signedtext_hex
    if (stream_msg == 0) {
        // Prepend CLIENT_NAME
        payload = String(CLIENT_NAME) + "_";
        // Append everything
        idx = 0;
    } else {
        // No need to send signature portion
        idx = (crypto_sign_BYTES)*2;
    }
    while (*(signedtext_hex + idx) != '\0') {
        payload.concat((char) *(signedtext_hex + idx));
        idx++;
    }

    sprintf(payload_len, "%0X\r\n", payload.length()+1);
    client.print(payload_len);
    payload.concat("\n\r\n");
    client.print(payload);

    Serial.print(F("stream_"));
    Serial.printf("%02d\n", stream_msg);

    stream_msg++;
}

void stream_add_json(
    const char *const query)
{
    uint8_t idx = 0;
    char json_out[256];
    handler_compose_json(query, json_out);

    while (*(json_out + idx) != '\0') {
        idx++;
    }

    *(json_out + idx) = '\n';
    *(json_out + idx + 1) = '\0';

    stream_add(json_out);
}

void stream_end(
    void)
{
    ensure_WiFi();
    uint8_t listen_attempts = 0;
    uint8_t buf[2];

    client.print("0\r\n");
    client.print("\r\n");
    stream_msg = 0;
    Serial.print(F("stream_end\n"));
    digitalWrite(LED_BLUE, HIGH);

    Serial.print(F("\nresponse\n"));
    while (!client.available()) {
        delay(500);
        listen_attempts++;
        if (listen_attempts > 60) {
            // 30 second timeout
            return;
        }
    }
    Serial.println();
    while (client.available()) {
        delay(10);
        client.read(buf, 1);
        Serial.printf("%c", *buf);
    }
}

// vim:fdm=syntax

