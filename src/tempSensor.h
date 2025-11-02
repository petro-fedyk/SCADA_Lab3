#ifndef TEMP_SENSOR_H
#define TEMP_SENSOR_H
#include <Arduino.h>

#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 0

float temperatureC = 0.0;

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void ds18b20_init()
{
    sensors.begin();
}

void read_temperature()
{
    sensors.requestTemperatures();

    temperatureC = sensors.getTempCByIndex(0);
    Serial.print("Celsius temperature: ");
    Serial.println(temperatureC);
}

#endif