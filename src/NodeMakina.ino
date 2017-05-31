#include <ESP8266WiFi.h>
#include <TickerScheduler.h> // Scheduler
#include <ArduinoJson.h>     // JSON emitter and parser

#define WIFI_SSID     "."
#define WIFI_PASSWORD "."
#define LED_RED       (uint8_t) 0
#define LED_BLUE      (uint8_t) 2

static TickerScheduler ts(1);

void connectToWiFi() {
  delay(10);
  digitalWrite(LED_RED, LOW);
  Serial.print(F("\nConnecting to "));
  Serial.print(F("WiFi"));
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }

  Serial.print(F("\n"));
  Serial.print(F("WiFi connected\n"));
  Serial.print(F("IP address: "));
  Serial.println(WiFi.localIP());
  digitalWrite(LED_RED, HIGH);
}

void setup() {
  Serial.begin(9600);

  // Set up blue LED
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_BLUE, HIGH);

  environment_setup();
  connectToWiFi();

  ts.add(0, 5000, routine_ADTS2017);
}

void loop() {
  ts.update();
}

// vim:fdm=syntax:sw=2
