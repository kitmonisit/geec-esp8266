#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <sodium.h>

#define HOST           "vast-lake-95491.herokuapp.com"
#define HTTP_HOST      "http://vast-lake-95491.herokuapp.com"
#define HTTP_PORT      80
// #define HOST           "192.168.22.4"
// #define HTTP_HOST      "http://192.168.22.4:5000"
// #define HTTP_PORT      5000
#define CLIENT_NAME    "node04_"
#define CLIENT_SK_HEX  "f0a6bd567547b1f2905b0bc0d7db4d903084d6d3883616ff1086f3b219743a14"
#define CLIENT_SSK_HEX "cb89b7d0a4d65ed8a8207220035f63b74352e0203a275859f577ce3db33d563d8e4ff2eb2a744b71f5e4f6f389fbcecea33966a765a5c13a622f109b78dabdec"
#define SERVER_PK_HEX  "5f8331082dc3f70428ac739a1a7981f911d7f0d3c0e0e583ad7f35c00faa141e"
#define TIMEOUT        (uint16_t) 10000
#define MAX_PLAINTEXT_LEN 130

static WiFiClient client;
static HTTPClient http;

static uint16_t      httpCode;
static String        cookie;
static unsigned char nonce[crypto_box_NONCEBYTES];
static uint8_t       stream_msg = 0;

static void func_random_buf(void * const buf, const size_t size);
static void print_hex(const unsigned char *bin, const size_t bin_len);

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

static void generate_own_nonce()
{
    if (stream_msg > 0) {
        func_random_buf(nonce, crypto_box_NONCEBYTES);
    }
}

static void *process_cookie(String pre_cookie)
{
    uint16_t len_cookie = (uint8_t) pre_cookie.indexOf(';', 0);
    cookie = pre_cookie.substring(0, len_cookie);
}

void stream_begin(void)
{
    digitalWrite(LED_BLUE, LOW);

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
    const char *const input)
{
    // input must be limited to 128 bytes total including null terminator
    String payload;

    char payload_len[7];
    uint16_t idx = 0;
    const uint8_t plaintext_len = MAX_PLAINTEXT_LEN;
    char plaintext[plaintext_len] = {0};

    // Prepend orig input length
    idx = 0;
    while (*(input+idx) != '\0') {
        idx++;
    }
    sprintf(plaintext, "%02X", idx);
    idx = 0;
    // Append input string
    strcpy(plaintext+2, input);

    unsigned long long nonce_ciphertext_len =
        crypto_box_NONCEBYTES
        + crypto_box_MACBYTES
        + plaintext_len;
    unsigned char      nonce_ciphertext[nonce_ciphertext_len];
    unsigned char      signedtext_hex[(crypto_sign_BYTES + nonce_ciphertext_len)*2 + 1];

    Serial.print(F("plaintext is\n  "));
    Serial.printf("%s\n", plaintext);
    // Encrypt
    generate_own_nonce();
    encrypt(nonce_ciphertext, nonce, plaintext, &plaintext_len);
    // Sign
    sign(signedtext_hex, nonce_ciphertext, &nonce_ciphertext_len);
    Serial.print(F("signedtext is\n  "));
    Serial.printf("%s\n", signedtext_hex);

    // Compose chunked data
    // https://en.wikipedia.org/wiki/Chunked_transfer_encoding#Example
    // payload: client_signedtext_hex
    if (stream_msg == 0) {
        // Prepend CLIENT_NAME
        payload = String(CLIENT_NAME);
        // Append everything
        idx = 0;
    } else {
        // No need to send signature
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
    digitalWrite(LED_BLUE, HIGH);

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

