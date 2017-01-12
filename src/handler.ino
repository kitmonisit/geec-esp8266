#include <ArduinoJson.h>

#define MAX_ATTEMPTS 10
#define ASCII_STX 0x02
#define ASCII_ETX 0x03
#define ASCII_ENQ 0x05
#define ASCII_ACK 0x06
#define ASCII_CR  0x0D

static void buf_clear(
    void)
{
    delay(1000);
    while (Serial.available() > 0) {
        Serial.read();
    }
}

static void buf_wait(
    void)
{
    while(!Serial.available());
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
        delay(100);
        if (Serial.available()) {
            inByte = Serial.read();
        }
    }
    attempts = 0;
    return 1; // success
}

static void handler_query(
    const char *const query)
{
    char out[100];
    uint8_t buf_contents = 0;
    memset(out, '\0', sizeof(out));

    *out = ASCII_STX;
    strcpy(out + 1, query);
    *(out + 1 + strlen(query)) = ASCII_ETX;

    Serial.write(out);
    buf_wait();

    return;
}

static uint8_t handler_ack(
    void)
{
    char out[2] = {ASCII_ACK, '\0'};
    uint8_t inByte = 0;

    buf_wait();

    if (Serial.available() > 0) {
        while (inByte != ASCII_ENQ) {
            inByte = Serial.read();
        }
        Serial.write(out);
        return 1; // success
    } else {
        return 0; // fail
    }
}

static uint8_t handler_read(
    char *const response)
{
    uint8_t idx = 0;

    buf_wait();

    while (1) {
        if (Serial.peek() == ASCII_STX) {
            Serial.read();
        } else if (Serial.peek() == ASCII_ETX) {
            Serial.read();
            break;
        } else {
            *(response + idx) = (char) Serial.read();
            idx++;
        }
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
            Serial.println(response);
            buf_clear();
        } else {
            Serial.println("FAIL handler_ack");
            buf_clear();
            sprintf(response, "FAIL handler ack");
            return 0; // fail
        }
    } else {
        Serial.println("FAIL handler_enq");
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

    handler_query_sequence(query, response);

    JsonObject& root = jsonBuffer.createObject();
    root["client"] = CLIENT_NAME;
    root["query"] = query;
    root["response"] = response;

    root.printTo(json_out, 256);
}

// vim:fdm=syntax

