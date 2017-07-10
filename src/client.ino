#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <sodium.h>

/*#define HOST              "35.194.135.178"*/
/*#define HTTP_HOST         "http://35.194.135.178"*/
#define HOST              "postman-echo.com"
#define HTTP_HOST         "https://postman-echo.com"
#define HTTP_ENDPOINT     "/post"
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

void stream_query_all()
{
  EspClass esp;
  ensure_WiFi();

  DynamicJsonBuffer jsonBuffer;

  JsonObject &root = jsonBuffer.createObject();
  stream_add_query_response_pair(root, "CDA");
  stream_add_query_response_pair(root, "CKS");
  stream_add_query_response_pair(root, "CZZ");

  JsonObject &node = root.createNestedObject("node");
  node["free RAM"] = esp.getFreeHeap();

  JsonObject &env = root.createNestedObject("env");
  env["temp"]     = environment_temperature();
  env["rh"]       = environment_humidity();

  // Prepend input length
  char content_length[5] = {0};
  sprintf(content_length, "Content-Length: %d\r\n\r\n", (uint8_t) root.measureLength());

  client.print(content_length);
  root.printTo(client);
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

static uint8_t stream_add_query_response_pair(
  JsonObject &object,
  const char *const query)
{
  char response[256];
  handler_query_sequence_dummy(query, response);
  JsonVariant variant = response;
  object[query] = variant.as<String>();
  return 0;
}

// vim:fdm=syntax:nowrap:sw=2
