#include <ESP8266WiFi.h>
#include "frozen.h" // JSON emitter and parser

#define WIFI_SSID     "."
#define WIFI_PASSWORD "."
#define LED_RED       (uint8_t) 0
#define LED_BLUE      (uint8_t) 2

/*EspClass esp;*/

void connectToWiFi() {
    delay(10);
    Serial.print(F("\nConnecting to "));
    Serial.print(F(WIFI_SSID));
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(F("."));
    }

    Serial.print(F("\n"));
    Serial.print(F("WiFi connected\n"));
    Serial.print(F("IP address: "));
    Serial.println(WiFi.localIP());
}

void setup() {
    Serial.begin(115200);

    // Set up blue LED
    pinMode(LED_BLUE, OUTPUT);
    digitalWrite(LED_BLUE, HIGH);

    connectToWiFi();
    stream_begin();
    stream_add("message1 ");
    stream_add("message2");
    stream_end();
}

void loop() {
    delay(1000);
    /*Serial.printf("%d free RAM\n", esp.getFreeHeap());*/
}

// vim:fdm=syntax

