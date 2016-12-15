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
}

void loop() {
    delay(10000);
    send_updates_to_cloud("e53e967e98d2b0bca38136803feabaaaa0b501fe38a702a17a6391ffc8bcf3227a9e43287d87241b542acb3642f5a97c2197e177b44c53863766e40b9ea7037bf683d687e889306e7d3d92bf34755018c0a649e4c45ba436dbb58d9f21fb826a99062bac261b8c9e6b79a1df44285ca3bf7fcca4b12b05761a95ca14505adbaef8c6307d17fd52feccd2ceb56119924ca5a4b7bfb134bb76037fedcdf9a0c1ac4583a88ef47f2e297f4b9ccb09e95d9178e072271a2065e31025d3330385630dd204e53ca988473f170aa87ec9de44e1237002794523deb208dfebd5c865a367ca9807cd4972614a3fa8e327ddf0b699134926129e55a7ba2079641a12fa72c25009d6bd36a2445e05f32b694818cec6a8f5f37716ea0729effc3ce989c0d5c0739b635d6f48a3fdb59cf2c6c2cb7ef4173fdda0d89a7c4f9dc6437e9cc8b3037c55ffffb8c27656c114337eda0963402114e2d07a79839776549bba5ce35ea37791d28d6c7cf44878fa5d5c64a5ebdb5df937df55ef9ac840d5658b0e08bbb9f85f3dd44f95bb1cc7fcf021a69c06ae7b974e46720a17321b72437a692430b11f3be0ce4573336440503e97c71a284611d7f972489d0e4cea36ab86322ca455887a070b8c980c43f6f6f88369f9724f3c6ae33931dd4c67b94292e940b7bb917f25a1c9e72e3f166b9151e2fef1c13913bd229043ad3b1158356bafbba3993408e758bff9b4a5e2ed16569b1fcbdf24e330dda93b549ac0ce79da923b3625ba73d759a1d60ccc89e37c60ba68912e1d3072c0ee4803cfdffb8ec6d097eb32933d6f14a21dbb9fc7ccd598dad411676f4e9d5fe291d7e383c678dd6698a696ca3de917470bd4bae1449a5d6163a87ebdbb805ee606b62d052d3634b07cc7f2ad1dc23114f6280ebbe995ade08123cf6b1f39130f932b3c266e19cd70259435b298612971970661eedbe37015bd76432243a05e29406e6ff7732a1c756e3b9ac08cbf67dc89910501d42603eb7d58df50f2a536835ae89ca3a0be7c90072133d229763cdbde1a4aa6eafe35731642c7afb351b4cabe86bd561250295e1b54f047e6aa76fbb575f52a8a01847b98453066f9");

    int ret = box();
    /*Serial.printf("%d free RAM\n", esp.getFreeHeap());*/
    Serial.printf("box ret is %d\n", ret);
}

// vim:fdm=syntax

