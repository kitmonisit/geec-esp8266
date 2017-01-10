#include <ArduinoJson.h>

#define ASCII_STX 0x02
#define ASCII_ETX 0x03
#define ASCII_ENQ 0x05
#define ASCII_ACK 0x06
#define ASCII_CR  0x0D

static void handler_enq(void)
{
    char out[3] = {ASCII_ENQ, ASCII_CR, '\0'};
    int inByte = 0;

    while (inByte != ASCII_ACK) {
        Serial.write(out);
        delay(100);
        if (Serial.available()) {
            inByte = Serial.read();
        }
    }
}

static void handler_query(
    const char *const query,
          char *const response)
{
    // Send ENQ to handler
    handler_enq();

    // Compose query to handler
    char out[100];
    memset(out, '\0', sizeof(out));

    *out = ASCII_STX;
    strcpy(out + 1, query);
    *(out + 1 + strlen(query)) = ASCII_ETX;

    // Send query to handler
    Serial.write(out);

    // Store response into response array
    delay(500);
    while (!Serial.available()) {
        delay(500);
    }
    Serial.readBytes(response, Serial.available());

    // The following line is just a placeholder
    // sprintf(response, "%02d", rand() % 50);
}


void handler_compose_json(
    const char *const query,
          char *const json_out)
{
    StaticJsonBuffer<256> jsonBuffer;
    char response[256];
    memset(response, '\0', sizeof(response));

    handler_query(query, response);

    JsonObject& root = jsonBuffer.createObject();
    root["client"] = CLIENT_NAME;
    root["query"] = query;
    root["response"] = response;

    root.printTo(json_out, 256);
}

// vim:fdm=syntax

