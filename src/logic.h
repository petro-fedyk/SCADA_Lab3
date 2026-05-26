#ifndef LOGIC_H
#define LOGIC_H
#include <Arduino.h>
#include "tempSensor.h"

#define IN1_PIN 13
#define IN2_PIN 14
#define RELAY_PIN 12

#define LED_RED_PIN 15
#define LED_YELLOW_PIN 2

const float MOTOR_ON_TEMP_C = 25.0;
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
int motorSpeedPercent = 0;
bool relayOn = false; // kept for UI compatibility: true when motor running
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
    if (relayOn)
    {
        digitalWrite(IN1_PIN, HIGH);
        digitalWrite(IN2_PIN, LOW);
    }
    else
    {
        digitalWrite(IN1_PIN, LOW);
        digitalWrite(IN2_PIN, LOW);
    }

    digitalWrite(RELAY_PIN, relayOn ? HIGH : LOW);

    digitalWrite(LED_RED_PIN, redLedOn ? HIGH : LOW);
    digitalWrite(LED_YELLOW_PIN, yellowLedOn ? HIGH : LOW);
}

void setup_logic()
{
    pinMode(IN1_PIN, OUTPUT);
    pinMode(IN2_PIN, OUTPUT);
    pinMode(RELAY_PIN, OUTPUT);
    pinMode(LED_RED_PIN, OUTPUT);
    pinMode(LED_YELLOW_PIN, OUTPUT);

    relayOn = false;
    motorSpeedPercent = 0;
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
        motorSpeedPercent = 0;
        powerPercent = 0;
        relayOn = false;
        redLedOn = false;
        yellowLedOn = false;
    }
    else
    {
        relayOn = temperatureC < MOTOR_ON_TEMP_C;
        motorSpeedPercent = relayOn ? 100 : 0;
        powerPercent = motorSpeedPercent;

        if (relayOn)
        {
            currentMode = MODE_FULL;
            redLedOn = true;
            yellowLedOn = false;
        }
        else
        {
            currentMode = MODE_OFF;
            redLedOn = false;
            yellowLedOn = false;
        }
    }

    applyOutputs();

    Serial.print("[STATUS] Temp=");
    Serial.print(temperatureC, 2);
    Serial.print(" C | Mode=");
    Serial.print(modeText());
    Serial.print(" | Motor=");
    Serial.print(motorSpeedPercent);
    Serial.print("% | Relay=");
    Serial.print(relayOn ? "ON" : "OFF");
    Serial.print(" | Red=");
    Serial.print(redLedOn ? "ON" : "OFF");
    Serial.print(" | Yellow=");
    Serial.println(yellowLedOn ? "ON" : "OFF");

    delay(LOGIC_LOOP_DELAY_MS);
}

#endif
