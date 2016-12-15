#include <ESP8266WiFi.h>
/*#include <sodium.h>*/

// using size_t 238,073 bytes program memory
//               32,496 bytes dynamic memory
// using char   238,073 bytes program memory
//               32,496 bytes dynamic memory

#define WIFI_SSID "....."
#define WIFI_PASSWORD "....."

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
    /*sodium_init();*/
}

void loop() {
    delay(10000);
    send_updates_to_cloud("Hello world");
}

// vim:fdm=syntax

