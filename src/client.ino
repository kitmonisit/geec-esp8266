#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <sodium.h>

#define HOST              "35.185.167.138"
#define HTTP_HOST         "http://35.185.167.138"
#define HTTP_PORT         80
// #define HOST              "192.168.22.5"
// #define HTTP_HOST         "http://192.168.22.5:5000"
// #define HTTP_PORT         5000
#define CLIENT_NAME       "handler3"
#define TIMEOUT           (uint16_t) 30000
#define MAX_PLAINTEXT_LEN 130
#define USE_ENCRYPTION    0

static WiFiClient client;

static void ensure_WiFi(void)
{
  if (WiFi.status() != WL_CONNECTED) {
    connectToWiFi();
  } else {
    return;
  }
}

void stream_begin(void)
{
  uint8_t status;
  // Serial.println(F("\n\nstream_begin"));
  digitalWrite(LED_BLUE, LOW);
  ensure_WiFi();

  String payload = String("POST ")
    + "/ HTTP/1.1\r\n"
    + "Host: " + HOST + "\r\n"
    + "Cache-Control: no-cache\r\n"
    + "Content-Type: application/json\r\n";

  while (!client.connected()) {
    // Serial.print("blah");
    status = client.connect(HOST, HTTP_PORT);
    // Serial.print(status);
  }
  Serial.print(payload);
  client.print(payload);
}

void stream_add(
  const char *const input)
{
  // input must be limited to 128 bytes total including null terminator
  ensure_WiFi();

  String payload;

  uint16_t idx = 0;
  const uint8_t plaintext_len = MAX_PLAINTEXT_LEN;
  char content_length[5] = {0};
  char plaintext[plaintext_len] = {0};

  // Prepend orig input length
  sprintf(content_length, "Content-Length: %d\r\n\r\n", (uint8_t) strlen(input));
  // Append input string
  strcpy(plaintext, input);

  // Compose chunked data
  // https://en.wikipedia.org/wiki/Chunked_transfer_encoding#Example

  payload = String(content_length);
  while (*(plaintext + idx) != '\0') {
    payload.concat((char) *(plaintext + idx));
    idx++;
  }

  // payload.concat("\n\r\n");
  client.print(payload);

  Serial.print(payload);
  // Serial.print(F("stream_"));
  // Serial.printf("%02d\n", stream_msg);
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

  *(json_out + idx) = '\0';

  stream_add(json_out);
}

void stream_end(
  void)
{
  ensure_WiFi();
  uint8_t listen_attempts = 0;
  uint8_t buf[2];

  // client.print("0\r\n");
  // client.print("\r\n");
  // Serial.print(F("stream_end\n"));
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
    client.read(buf, 1);
    Serial.printf("%c", *buf);
  }
}

// vim:fdm=syntax:nowrap:sw=2
