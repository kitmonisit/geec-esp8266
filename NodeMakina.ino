#include <ESP8266WiFi.h>

#define WIFI_SSID "....."
#define WIFI_PASSWORD "....."

void connectToWiFi() {
    delay(10);
    Serial.print("\nConnecting to ");
    Serial.println(WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}

void setup() {
    Serial.begin(115200);
    connectToWiFi();
}

void loop() {
    delay(10000);
    send_updates_to_cloud("Hello world");
}

// vim:fdm=syntax

