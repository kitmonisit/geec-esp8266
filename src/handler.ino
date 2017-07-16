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

static uint8_t handler_query_sequence_dummy(
  const char *const query,
        char *const response)
{
  char prefix[] = "Your query is ";
  strcat(prefix, query);
  sprintf(response, prefix);
  return 0;
}

void handler_compose_json(
        char  *json_out,
        char query_array[6][64])
{
  uint8_t idx = 0;
  DynamicJsonBuffer jsonBuffer;

  JsonObject &root = jsonBuffer.createObject();

  while (idx < 5) {
    handler_mod_json(root, query_array[idx]);
    idx++;
  }

  JsonObject &node  = root.createNestedObject("node");
  node["free RAM"]  = node_get_free_RAM();
  node["timestamp"] = 0;

  JsonObject &env   = root.createNestedObject("env");
  env["temp"]       = environment_temperature();
  env["rh"]         = environment_humidity();

  root.printTo(json_out, root.measureLength());
  return;
}

void handler_mod_json(
  JsonObject &object,
  const char *key)
{
  char value[256];
  handler_query_sequence_dummy(key, value);
  // printf("%s: %s\n", key, value);

  JsonVariant variant = value;
  Serial.print(""); // Need this so that `port status?` query does not freeze. WEIRD.
  object.set(key, variant.as<String>());
  return;
}

// vim:fdm=syntax:sw=2
