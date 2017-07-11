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
  ensure_WiFi();

  DynamicJsonBuffer jsonBuffer;

  JsonObject &root = jsonBuffer.createObject();
  stream_add_query_response_pair(root, "CA");
  stream_add_query_response_pair(root, "CAS");
  stream_add_query_response_pair(root, "CB");
  stream_add_query_response_pair(root, "CD");
  stream_add_query_response_pair(root, "CDA");
  stream_add_query_response_pair(root, "CDC");
  stream_add_query_response_pair(root, "CDI");
  stream_add_query_response_pair(root, "CDN");
  stream_add_query_response_pair(root, "CDNH");
  stream_add_query_response_pair(root, "CDNL");
  stream_add_query_response_pair(root, "CDNU");
  stream_add_query_response_pair(root, "CDP");
  stream_add_query_response_pair(root, "CDS");
  stream_add_query_response_pair(root, "CDZ");
  stream_add_query_response_pair(root, "CE");
  stream_add_query_response_pair(root, "CEC");
  stream_add_query_response_pair(root, "CER");
  stream_add_query_response_pair(root, "CF");
  stream_add_query_response_pair(root, "CK");
  stream_add_query_response_pair(root, "CKA");
  stream_add_query_response_pair(root, "CKLD");
  stream_add_query_response_pair(root, "CKLS");
  stream_add_query_response_pair(root, "CKS");
  stream_add_query_response_pair(root, "CKTI");
  stream_add_query_response_pair(root, "CML");
  stream_add_query_response_pair(root, "CMM");
  stream_add_query_response_pair(root, "CN");
  stream_add_query_response_pair(root, "COB");
  stream_add_query_response_pair(root, "COF");
  stream_add_query_response_pair(root, "CQC");
  stream_add_query_response_pair(root, "CQI");
  stream_add_query_response_pair(root, "CQL");
  stream_add_query_response_pair(root, "CQO");
  stream_add_query_response_pair(root, "CQP");
  stream_add_query_response_pair(root, "CQU");
  stream_add_query_response_pair(root, "CSO");
  stream_add_query_response_pair(root, "CSOT");
  stream_add_query_response_pair(root, "CSR");
  stream_add_query_response_pair(root, "CSS");
  stream_add_query_response_pair(root, "CSSA");
  stream_add_query_response_pair(root, "CSS1T");
  stream_add_query_response_pair(root, "CSS2T");
  stream_add_query_response_pair(root, "CST");
  stream_add_query_response_pair(root, "CSW");
  stream_add_query_response_pair(root, "CSWA");
  stream_add_query_response_pair(root, "CVC");
  stream_add_query_response_pair(root, "CZASE");
  stream_add_query_response_pair(root, "CZD");
  stream_add_query_response_pair(root, "CZETP");
  stream_add_query_response_pair(root, "CZ 1 port status?");
  stream_add_query_response_pair(root, "CZ 2 port status?");
  stream_add_query_response_pair(root, "CZ 3 port status?");
  stream_add_query_response_pair(root, "CZSPA");
  stream_add_query_response_pair(root, "CZSPC");
  stream_add_query_response_pair(root, "CZTCBASE");
  stream_add_query_response_pair(root, "CZTCOR");
  stream_add_query_response_pair(root, "CZTPA");
  stream_add_query_response_pair(root, "CZTPC");
  stream_add_query_response_pair(root, "CZPWD");
  stream_add_query_response_pair(root, "DD");
  stream_add_query_response_pair(root, "DF");
  stream_add_query_response_pair(root, "DFA");
  stream_add_query_response_pair(root, "DM0");
  stream_add_query_response_pair(root, "DM1");
  stream_add_query_response_pair(root, "DM2");
  stream_add_query_response_pair(root, "DM3");
  stream_add_query_response_pair(root, "DM4");
  stream_add_query_response_pair(root, "DM5");
  stream_add_query_response_pair(root, "DM6");
  stream_add_query_response_pair(root, "DM7");
  stream_add_query_response_pair(root, "DM8");
  stream_add_query_response_pair(root, "DM9");
  stream_add_query_response_pair(root, "DM10");
  stream_add_query_response_pair(root, "DM11");
  stream_add_query_response_pair(root, "DM12");
  stream_add_query_response_pair(root, "DN0");
  stream_add_query_response_pair(root, "DN1");
  stream_add_query_response_pair(root, "DN2");
  stream_add_query_response_pair(root, "DN3");
  stream_add_query_response_pair(root, "DPY");
  stream_add_query_response_pair(root, "DPYD");
  stream_add_query_response_pair(root, "DPYP");
  stream_add_query_response_pair(root, "DPYR");
  stream_add_query_response_pair(root, "DPYS");
  stream_add_query_response_pair(root, "DPYT");
  stream_add_query_response_pair(root, "DPYY");
  stream_add_query_response_pair(root, "DT0");
  stream_add_query_response_pair(root, "DT1");
  stream_add_query_response_pair(root, "DT2");
  stream_add_query_response_pair(root, "DT3");
  stream_add_query_response_pair(root, "DT4");
  stream_add_query_response_pair(root, "DT5");
  stream_add_query_response_pair(root, "DT6");
  stream_add_query_response_pair(root, "DT7");
  stream_add_query_response_pair(root, "DT8");
  stream_add_query_response_pair(root, "DT9");
  stream_add_query_response_pair(root, "DT10");
  stream_add_query_response_pair(root, "DV 0");
  stream_add_query_response_pair(root, "DV 1");
  stream_add_query_response_pair(root, "DV 2");
  stream_add_query_response_pair(root, "DV 3");
  stream_add_query_response_pair(root, "DV 4");
  stream_add_query_response_pair(root, "DV 5");
  stream_add_query_response_pair(root, "DV 6");
  stream_add_query_response_pair(root, "DV 7");
  stream_add_query_response_pair(root, "DV 8");
  stream_add_query_response_pair(root, "DV 9");
  stream_add_query_response_pair(root, "DV 10");
  stream_add_query_response_pair(root, "DV 11");
  stream_add_query_response_pair(root, "DV 12");
  stream_add_query_response_pair(root, "DV 13");
  stream_add_query_response_pair(root, "DV 14");
  stream_add_query_response_pair(root, "DV 15");
  stream_add_query_response_pair(root, "DV 16");
  stream_add_query_response_pair(root, "DV 17");
  stream_add_query_response_pair(root, "DV 18");
  stream_add_query_response_pair(root, "DV 19");
  stream_add_query_response_pair(root, "DV 20");
  stream_add_query_response_pair(root, "DV 21");
  stream_add_query_response_pair(root, "DV 22");
  stream_add_query_response_pair(root, "DV 23");
  stream_add_query_response_pair(root, "DV 24");
  stream_add_query_response_pair(root, "DV 25");
  stream_add_query_response_pair(root, "DV 26");
  stream_add_query_response_pair(root, "DV 27");
  stream_add_query_response_pair(root, "DV 28");
  stream_add_query_response_pair(root, "DV 29");
  stream_add_query_response_pair(root, "DV 30");
  stream_add_query_response_pair(root, "DV 31");
  stream_add_query_response_pair(root, "DY");
  stream_add_query_response_pair(root, "FAR");
  stream_add_query_response_pair(root, "HVRC");
  stream_add_query_response_pair(root, "HVRL");
  stream_add_query_response_pair(root, "HVRO");
  stream_add_query_response_pair(root, "HVRU");

  JsonObject &node = root.createNestedObject("node");
  node["free RAM"] = node_get_free_RAM();

  JsonObject &env = root.createNestedObject("env");
  env["temp"]     = environment_temperature();
  env["rh"]       = environment_humidity();

  // Prepend input length
  char content_length[5] = {0};
  sprintf(content_length, "Content-Length: %d\r\n\r\n", (uint8_t) root.measureLength());

  client.print(content_length);
  root.prettyPrintTo(Serial);
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
