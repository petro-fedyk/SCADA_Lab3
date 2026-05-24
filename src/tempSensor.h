#ifndef TEMP_SENSOR_H
#define TEMP_SENSOR_H
#include <Arduino.h>

#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 0

const float CLARITY_MAX = 100.0;
const float CLARITY_MIN = 0.0;
const float CLARITY_DROP_PER_DEGREE = 2.0;

const float PH_MIN = 5.5;
const float PH_MAX = 8.5;
const float PH_BASE = 7.0;
const float PH_SLOPE_PER_DEGREE = 0.03; // pH changes with temperature

float temperatureC = 0.0;
float waterClarity = CLARITY_MAX;
float waterPh = PH_BASE;

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void ds18b20_init()
{
    sensors.begin();
}

void update_water_quality()
{
    waterClarity = CLARITY_MAX - temperatureC * CLARITY_DROP_PER_DEGREE;
    waterClarity = constrain(waterClarity, CLARITY_MIN, CLARITY_MAX);

    float phRaw = PH_BASE - (temperatureC - 25.0) * PH_SLOPE_PER_DEGREE;
    waterPh = constrain(phRaw, PH_MIN, PH_MAX);
}

void read_temperature()
{
    sensors.requestTemperatures();

    temperatureC = sensors.getTempCByIndex(0);
    update_water_quality();
    Serial.print("Celsius temperature: ");
    Serial.println(temperatureC);
    Serial.print("Water clarity: ");
    Serial.print(waterClarity, 1);
    Serial.println(" %");
    Serial.print("Water pH: ");
    Serial.println(waterPh, 2);
}

#endif