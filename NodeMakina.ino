#include <ESP8266WiFi.h>
#include <sodium.h>
#include "frozen.h" // JSON emitter and parser
#include "ESP8266TrueRandom.h" // true random number generator

#define WIFI_SSID "."
#define WIFI_PASSWORD "."

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
    connectToWiFi();
    send_updates_to_cloud("Hello world, encryption works! Yeaahh!");
}

void loop() {
    delay(1000);

    /*int ret = box();*/
    /*Serial.printf("%d free RAM\n", esp.getFreeHeap());*/
    /*Serial.printf("box ret is %d\n", ret);*/
}

// vim:fdm=syntax

