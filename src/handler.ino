#include <ArduinoJson.h>

static void handler_query(
    const char *const query,
          char *const response)
{
    // Send query to handler
    // Store response into response array

    // The following line is just a placeholder
    sprintf(response, "%02d", rand() % 50);
}


void handler_compose_json(
    const char *const query,
          char *const json_out)
{
    StaticJsonBuffer<256> jsonBuffer;
    char response[256];

    handler_query(query, response);

    JsonObject& root = jsonBuffer.createObject();
    root["client"] = CLIENT_NAME;
    root["query"] = query;
    root["response"] = response;

    root.printTo(json_out, 256);
}

