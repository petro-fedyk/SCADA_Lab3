#ifndef POWER_SENSOR_H
#define POWER_SENSOR_H

#include <Arduino.h>
#include <Wire.h>
#include <INA226_WE.h>
#define I2C_ADDRESS 0x40

INA226_WE ina226(I2C_ADDRESS);

float voltage = 0;
float current = 0;
float power = 0;
uint8_t batteryLevel = 0;

const float voltageTable[] = {
    4.20, 4.10, 4.00, 3.92, 3.87, 3.83, 3.79, 3.75, 3.70, 3.60, 3.50, 3.20};
const uint8_t percentTable[] = {
    100, 90, 80, 70, 60, 50, 40, 30, 20, 10, 5, 0};

void ina226_init(uint8_t address)
{
    Wire.begin();
    ina226.init();
    ina226.setResistorRange(0.1, 1.3);
    ina226.setCorrectionFactor(0.93);
    Serial.println("INA226 ready");
}

uint8_t voltageToPercent(float v)
{
    // Якщо напруга вище за 4.2В — 100%
    if (v >= voltageTable[0])
        return 100;
    // Якщо нижче за 3.2В — 0%
    if (v <= voltageTable[11])
        return 0;

    // Пошук діапазону
    for (int i = 0; i < 11; i++)
    {
        if (v <= voltageTable[i] && v > voltageTable[i + 1])
        {
            // Лінійна інтерполяція
            float x1 = voltageTable[i];
            float x2 = voltageTable[i + 1];
            float y1 = percentTable[i];
            float y2 = percentTable[i + 1];
            float percent = y1 + (v - x1) * (y2 - y1) / (x2 - x1);
            return constrain((int)percent, 0, 100);
        }
    }
    return 0;
}

void ina226_read()
{
    ina226.readAndClearFlags();
    voltage = ina226.getBusVoltage_V();
    current = ina226.getCurrent_mA();
    power = ina226.getBusPower();

    // Обчислюємо рівень заряду батареї на основі напруги
    batteryLevel = voltageToPercent(voltage);

    Serial.print("U[V]=");
    Serial.print(voltage);
    Serial.print("  I[mA]=");
    Serial.print(current);
    Serial.print("  P[mW]=");
    Serial.println(power);
    Serial.print("  Battery[%]=");
    Serial.println(batteryLevel);
}

#endif
