#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTTYPE DHT22        // DHT 22 (AM2302)
#define DHTPIN  (uint8_t) 14 // Pin which is connected to the DHT sensor.
DHT_Unified dht(DHTPIN, DHTTYPE);

void environment_setup()
{
    dht.begin();
}

float environment_temperature() {
    sensors_event_t event;

    dht.temperature().getEvent(&event);
    if (isnan(event.temperature)) {
        return 0;
    } else {
        return event.temperature;
    }
}

float environment_humidity() {
    sensors_event_t event;

    dht.humidity().getEvent(&event);
    if (isnan(event.temperature)) {
        return 0;
    } else {
        return event.relative_humidity;
    }
}

// vim:fdm=syntax

