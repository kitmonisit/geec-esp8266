#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <sodium.h>

#define HOST              "35.185.157.106"
#define HTTP_HOST         "http://35.185.157.106"
#define HTTP_ENDPOINT     "/"
/*#define HOST              "postman-echo.com"*/
/*#define HTTP_HOST         "https://postman-echo.com"*/
/*#define HTTP_ENDPOINT     "/post"*/
#define HTTP_PORT         80
// #define HOST              "192.168.22.5"
// #define HTTP_HOST         "http://192.168.22.5:5000"
// #define HTTP_PORT         5000
#define CLIENT_NAME       "handler3"
#define TIMEOUT           (uint16_t) 30000
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
  digitalWrite(LED_BLUE, LOW);
  ensure_WiFi();

  String payload = String("POST ")
    + HTTP_ENDPOINT + " HTTP/1.1\r\n"
    + "Host: " + HOST + "\r\n"
    + "Cache-Control: no-cache\r\n"
    + "Content-Type: application/json\r\n";

  while (!client.connected()) {
    status = client.connect(HOST, HTTP_PORT);
  }
  client.print(payload);
}

void stream_add(
  const char *const input)
{
  ensure_WiFi();
  char content_length[5] = {0};

  // Prepend input length
  sprintf(content_length, "Content-Length: %d\r\n\r\n", (uint8_t) strlen(input));

  client.print(content_length);
  client.print(input);
}

void stream_query(
  char query_array[6][64])
{
  ensure_WiFi();
  char json_out[1024];

  handler_compose_json(json_out, query_array);

  // Prepend input length
  char content_length[64] = {0};
  sprintf(content_length, "Content-Length: %d\r\n\r\n", (uint16_t) strlen(json_out));

  client.print(content_length);
  client.print(json_out);
  return;
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
  Serial.println();
  return;
}

// vim:fdm=syntax:wrap:sw=2
