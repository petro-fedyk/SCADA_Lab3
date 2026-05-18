#ifndef LOGIC_H
#define LOGIC_H
#include <Arduino.h>
#include "waterLevelSensor.h"

#define ON_PIN 15
#define OFF_PIN 13
#define ALARM_PIN 12
#define RELAY_PIN 16

// Timing and state constants (replace magic numbers)
const unsigned long ALARM_BLINK_INTERVAL_MS = 500; // ms interval for alarm blinking
const unsigned long LOGIC_LOOP_DELAY_MS = 150;     // ms delay at end of logic loop

// Auto-pump thresholds (water level in %)
const float AUTO_PUMP_LOW = 20.0f;
const float AUTO_PUMP_HIGH = 80.0f;

// Alarm thresholds (water level in %)
const float ALARM_LOW = 10.0f;
const float ALARM_HIGH = 90.0f;

bool pumpOn = false;
bool auto_pump = true;
int alarm_indc = 0; // 0 = no alarm, 1 = low, 2 = high

void setup_logic()
{
    pinMode(ON_PIN, OUTPUT);
    pinMode(OFF_PIN, OUTPUT);
    pinMode(ALARM_PIN, OUTPUT);
    pinMode(RELAY_PIN, OUTPUT);

    digitalWrite(ON_PIN, LOW);
    digitalWrite(OFF_PIN, HIGH);
    digitalWrite(ALARM_PIN, LOW);
    digitalWrite(RELAY_PIN, LOW);

    Serial.println("[LOGIC] Setup complete");
}

unsigned long alarmBlinkLast = 0;
bool alarmLedState = false;

void loop_logic()
{
    // --- Керування станом ---
    if (auto_pump)
    {
        if (water_level <= AUTO_PUMP_LOW)
            pumpOn = true;
        else if (water_level >= AUTO_PUMP_HIGH)
            pumpOn = false;
    }

    if (pumpOn)
    {
        digitalWrite(ON_PIN, HIGH);
        digitalWrite(OFF_PIN, LOW);
        digitalWrite(RELAY_PIN, HIGH);
    }
    else
    {
        digitalWrite(ON_PIN, LOW);
        digitalWrite(OFF_PIN, HIGH);
        digitalWrite(RELAY_PIN, LOW);
    }

    // --- Логіка аварії ---
    alarm_indc = 0;
    if (water_level <= ALARM_LOW)
        alarm_indc = 1;
    else if (water_level >= ALARM_HIGH)
        alarm_indc = 2;

    if (alarm_indc != 0)
    {
        unsigned long now = millis();
        if (now - alarmBlinkLast >= ALARM_BLINK_INTERVAL_MS)
        {
            alarmBlinkLast = now;
            alarmLedState = !alarmLedState;
            digitalWrite(ALARM_PIN, alarmLedState ? HIGH : LOW);
        }
    }
    else
    {
        alarmLedState = false;
        digitalWrite(ALARM_PIN, LOW);
    }

    // --- Статус логування ---
    Serial.print("[STATUS] Water=");
    Serial.print(water_level, 1);
    Serial.print("% | Pump=");
    Serial.print(pumpOn ? "ON" : "OFF");
    Serial.print(" | Auto=");
    Serial.print(auto_pump ? "ON" : "OFF");
    Serial.print(" | Alarm=");
    Serial.print(alarm_indc);
    Serial.print(" | ON_PIN=");
    Serial.print(digitalRead(ON_PIN));
    Serial.print(" | OFF_PIN=");
    Serial.print(digitalRead(OFF_PIN));
    Serial.print(" | RELAY_PIN=");
    Serial.print(digitalRead(RELAY_PIN));
    Serial.print(" | ALARM_PIN=");
    Serial.println(digitalRead(ALARM_PIN));

    delay(LOGIC_LOOP_DELAY_MS);
}

#endif
