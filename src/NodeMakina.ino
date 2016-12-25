#include <ESP8266WiFi.h>
#include <sodium.h>
#include "frozen.h" // JSON emitter and parser

#define WIFI_SSID     "."
#define WIFI_PASSWORD "."
#define LED_RED       (uint8_t) 0
#define LED_BLUE      (uint8_t) 2

// EspClass esp;

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
    Serial.begin(115200);

    // Set up blue LED
    pinMode(LED_RED, OUTPUT);
    pinMode(LED_BLUE, OUTPUT);
    digitalWrite(LED_RED, HIGH);
    digitalWrite(LED_BLUE, HIGH);

    connectToWiFi();

    randombytes_set_implementation(&impl);
    sodium_init();
}

void loop() {
    delay(1000);
    // Serial.printf("%d free RAM\n", esp.getFreeHeap());
    stream_begin();
    stream_add("Bayang Magiliw,\n");
    stream_add("Perlas ng Silanganan\n");
    stream_add("Alab ng puso\n");
    stream_add("Sa Dibdib mo'y buhay.\n");
    stream_add("\n");
    stream_add("Lupang Hinirang,\n");
    stream_add("Duyan ka ng magiting,\n");
    stream_add("Sa manlulupig\n");
    stream_add("Di ka pasisiil\n");
    stream_add("\n");
    stream_add("Sa dagat at bundok,\n");
    stream_add("Sa simoy at sa langit mong bughaw,\n");
    stream_add("May dilag ang tula\n");
    stream_add("At awit sa paglayang minamahal.\n");
    stream_add("\n");
    stream_add("Ang kislap ng watawat mo'y\n");
    stream_add("Tagumpay na nagniningning;\n");
    stream_add("Ang bituin at araw niya,\n");
    stream_add("Kailan pa ma'y di magdidilim\n");
    stream_add("\n");
    stream_add("Lupa ng araw, ng luwalhati't pagsinta,\n");
    stream_add("Buhay ay langit sa piling mo;\n");
    stream_add("Aming ligaya na pag may mang-aapi,\n");
    stream_add("Ang mamatay nang dahil sa iyo.\n");
    stream_end();
}

// vim:fdm=syntax

