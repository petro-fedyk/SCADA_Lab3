#ifndef TEMP_SENSOR_H
#define TEMP_SENSOR_H
#include <Arduino.h>

#include <DHT.h>

#define DHT_PIN 0
#define DHT_TYPE DHT22

float temperatureC = 0.0;
float humidityPct = 0.0;

DHT dht(DHT_PIN, DHT_TYPE);

void dht22_init()
{
    dht.begin();
}

void read_temperature()
{
    float t = dht.readTemperature();
    float h = dht.readHumidity();
    if (!isnan(t))
        temperatureC = t;
    if (!isnan(h))
        humidityPct = h;

    Serial.print("Celsius temperature: ");
    Serial.println(temperatureC);
    Serial.print("Humidity: ");
    Serial.println(humidityPct);
}

#endif