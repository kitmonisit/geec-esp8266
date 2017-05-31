#include <ArduinoJson.h>

#define MAX_ATTEMPTS 10
#define ASCII_STX 0x02
#define ASCII_ETX 0x03
#define ASCII_ENQ 0x05
#define ASCII_ACK 0x06
#define ASCII_LF  0x0A
#define ASCII_CR  0x0D

static void buf_clear(
    void)
{
    delay(100);
    while (Serial.available() > 0) {
        Serial.read();
    }
}

static void buf_wait(
    uint8_t seconds = 10)
{
    uint8_t idx = 0;
    while (!Serial.available()) {
        delay(100);
        if (idx > seconds-1) {
            break;
        }
        idx++;
    }
    return;
}

static uint8_t handler_enq(
    void)
{
    char out[2] = {ASCII_ENQ, '\0'};
    uint8_t attempts = 0;
    uint8_t inByte = 0;

    buf_clear();
    while (inByte != ASCII_ACK) {
        attempts++;
        if (attempts > MAX_ATTEMPTS) {
            return 0; // fail
        }
        Serial.write(out);
        buf_wait();
        if (Serial.available()) {
            inByte = Serial.read();
        }
    }
    buf_clear();
    return 1; // success
}

static void handler_query(
    const char *const query)
{
  char out[100];
  memset(out, '\0', sizeof(out));

  *out = ASCII_STX;
  strcpy(out + 1, query);
  *(out + 1 + strlen(query)) = ASCII_ETX;

  Serial.write(out);
}

static uint8_t handler_ack(
  void)
{
  char out[2] = {ASCII_ACK, '\0'};
  uint8_t inByte = 0;

  buf_wait();
  inByte = (char) Serial.read();
  if (inByte == ASCII_ENQ) {
    Serial.write(out);
    return 1; // success
  } else {
    return 0; // fail
  }
}

static uint8_t handler_read(
  char *const response)
{
  char inByte;
  uint8_t idx = 0;

  buf_wait();
  while (Serial.available()) {
    inByte = (char) Serial.read();
    if (inByte == ASCII_STX || inByte == ASCII_CR || inByte == ASCII_LF) {
    } else if (inByte == ASCII_ETX) {
      *(response + idx) = '\0';
      break;
    } else {
      *(response + idx) = inByte;
      idx++;
    }
    buf_wait();
  }

  return 1; // success
}

static uint8_t handler_query_sequence(
  const char *const query,
        char *const response)
{
  if (handler_enq()) {
    handler_query(query);
    if (handler_ack()) {
      handler_read(response);
      handler_ack();
      buf_clear();
    } else {
      // Serial.println("FAIL handler_ack");
      buf_clear();
      sprintf(response, "FAIL handler ack");
      return 0; // fail
    }
  } else {
    // Serial.println("FAIL handler_enq");
    buf_clear();
    sprintf(response, "FAIL handler_enq");
    return 0; // fail
  }
}

void handler_compose_json(
  const char *const query,
        char *const json_out)
{
  StaticJsonBuffer<256> jsonBuffer;
  char response[256];
  memset(response, '\0', sizeof(response));

  // handler_query_sequence(query, response);

  JsonObject& root = jsonBuffer.createObject();

  JsonObject& handler = root.createNestedObject("handler");
  handler["ID"] = CLIENT_NAME;
  handler["type"] = "Flex";
  JsonObject& sensors = handler.createNestedObject("sensors");
  sensors["temp"] = environment_temperature();
  sensors["rh"] = environment_humidity();
  JsonObject& metadata = handler.createNestedObject("metadata");
  metadata["status"] = "repair";
  metadata["error"] = "jam";

  root.printTo(json_out, 256);
}

// vim:fdm=syntax:sw=2
