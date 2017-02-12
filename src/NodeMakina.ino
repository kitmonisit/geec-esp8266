#include <ESP8266WiFi.h>
#include <TickerScheduler.h> // Scheduler
#include <sodium.h>          // Encryption
#include <ArduinoJson.h>     // JSON emitter and parser

/////TEMP & HUMIDITY SENSOR SETUP
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#define DHTTYPE           DHT22     // DHT 22 (AM2302)
#define DHTPIN            14         // Pin which is connected to the DHT sensor.
DHT_Unified dht(DHTPIN, DHTTYPE);


#define WIFI_SSID     "."
#define WIFI_PASSWORD "."
#define LED_RED       (uint8_t) 0
#define LED_BLUE      (uint8_t) 2

uint32_t delayMS;
static TickerScheduler ts(1);

static const char *func_name(void);
static void        func_random_buf(void * const buf, const size_t size);

static struct randombytes_implementation impl = {
    SODIUM_C99(.implementation_name =) func_name,
    SODIUM_C99(.random =) NULL,
    SODIUM_C99(.stir =) NULL,
    SODIUM_C99(.uniform =) NULL,
    SODIUM_C99(.buf =) func_random_buf,
    SODIUM_C99(.close =) NULL,
};

void connectToWiFi() {
    delay(10);
    digitalWrite(LED_RED, LOW);
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
    digitalWrite(LED_RED, HIGH);
}

void setup() {
    Serial.begin(9600);
    dht.begin();
    sensor_t sensor;
    delayMS = sensor.min_delay / 1000;
    // Set up blue LED
    pinMode(LED_RED, OUTPUT);
    pinMode(LED_BLUE, OUTPUT);
    digitalWrite(LED_RED, HIGH);
    digitalWrite(LED_BLUE, HIGH);

    connectToWiFi();

    randombytes_set_implementation(&impl);
    sodium_init();

    ts.add(0, 1000, routine_Demo);
}

void loop() {
    ts.update();
}

// vim:fdm=syntax
