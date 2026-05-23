#ifndef LOGIC_H
#define LOGIC_H
#include <Arduino.h>
#include "tempSensor.h"

#define RELAY_PIN 14
#define LED_RED_PIN 15
#define LED_YELLOW_PIN 13

const float TEMP_HIGH_C = 28.0;
const float TEMP_MID_C = 18.0;
const float TEMP_SENSOR_INVALID_C = -100.0;

const unsigned long LOGIC_LOOP_DELAY_MS = 150; // ms delay at end of logic loop

enum WorkMode
{
    MODE_OFF = 0,
    MODE_HALF = 1,
    MODE_FULL = 2
};

WorkMode currentMode = MODE_OFF;
int powerPercent = 0;
bool relayOn = false;
bool redLedOn = false;
bool yellowLedOn = false;

const char *modeText()
{
    switch (currentMode)
    {
    case MODE_FULL:
        return "FULL";
    case MODE_HALF:
        return "HALF";
    default:
        return "OFF";
    }
}

void applyOutputs()
{
    // Relay is active-low: LOW closes, HIGH opens
    digitalWrite(RELAY_PIN, relayOn ? LOW : HIGH);
    digitalWrite(LED_RED_PIN, redLedOn ? HIGH : LOW);
    digitalWrite(LED_YELLOW_PIN, yellowLedOn ? HIGH : LOW);
}

void setup_logic()
{
    pinMode(RELAY_PIN, OUTPUT);
    pinMode(LED_RED_PIN, OUTPUT);
    pinMode(LED_YELLOW_PIN, OUTPUT);

    relayOn = false;
    redLedOn = false;
    yellowLedOn = false;
    applyOutputs();

    Serial.println("[LOGIC] Setup complete");
}

void loop_logic()
{
    if (temperatureC < TEMP_SENSOR_INVALID_C)
    {
        currentMode = MODE_OFF;
        powerPercent = 0;
        relayOn = false;
        redLedOn = false;
        yellowLedOn = false;
    }
    else if (temperatureC > TEMP_HIGH_C)
    {
        currentMode = MODE_FULL;
        powerPercent = 80;
        relayOn = true;
        redLedOn = true;
        yellowLedOn = false;
    }
    else if (temperatureC >= TEMP_MID_C)
    {
        currentMode = MODE_HALF;
        powerPercent = 50;
        relayOn = false;
        redLedOn = false;
        yellowLedOn = true;
    }
    else
    {
        currentMode = MODE_OFF;
        powerPercent = 0;
        relayOn = false;
        redLedOn = false;
        yellowLedOn = false;
    }

    applyOutputs();

    Serial.print("[STATUS] Temp=");
    Serial.print(temperatureC, 2);
    Serial.print(" C | Mode=");
    Serial.print(modeText());
    Serial.print(" | Power=");
    Serial.print(powerPercent);
    Serial.print("% | Relay=");
    Serial.print(relayOn ? "ON" : "OFF");
    Serial.print(" | Red=");
    Serial.print(redLedOn ? "ON" : "OFF");
    Serial.print(" | Yellow=");
    Serial.println(yellowLedOn ? "ON" : "OFF");

    delay(LOGIC_LOOP_DELAY_MS);
}

#endif
