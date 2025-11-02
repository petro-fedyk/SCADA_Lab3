#ifndef LOGIC_H
#define LOGIC_H
#include <Arduino.h>
#include "powerSensor.h"
#include "tempSensor.h"

#define BTN_PIN 14
#define ON_PIN 15
#define OFF_PIN 13
#define ALARM_PIN 12

void setup_logic()
{
    pinMode(BTN_PIN, INPUT_PULLUP);
    pinMode(ON_PIN, OUTPUT);
    pinMode(OFF_PIN, OUTPUT);
    pinMode(ALARM_PIN, OUTPUT);

    digitalWrite(ON_PIN, LOW);
    digitalWrite(OFF_PIN, HIGH);
    digitalWrite(ALARM_PIN, LOW);

    Serial.println("[LOGIC] Setup complete");
}

bool stationOn = false;
int lastReading = HIGH;
int stableState = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

unsigned long alarmBlinkLast = 0;
bool alarmLedState = false;

void loop_logic()
{
    int reading = digitalRead(BTN_PIN);

    // Якщо стан змінився - перезапускаємо debounce таймер
    if (reading != lastReading)
    {
        lastDebounceTime = millis();
    }

    // Якщо стабільний новий стан довше ніж debounceDelay
    if ((millis() - lastDebounceTime) > debounceDelay)
    {
        if (reading != stableState)
        {
            stableState = reading;

            // Натискання — перехід HIGH -> LOW
            if (stableState == LOW)
            {
                stationOn = !stationOn;
                Serial.print("[BUTTON] Press detected → Station state: ");
                Serial.println(stationOn ? "ON" : "OFF");
            }
        }
    }

    lastReading = reading;

    // --- Керування станом ---
    if (stationOn)
    {
        digitalWrite(ON_PIN, HIGH);
        digitalWrite(OFF_PIN, LOW);
    }
    else
    {
        digitalWrite(ON_PIN, LOW);
        digitalWrite(OFF_PIN, HIGH);
    }

    // --- Логіка аварії ---
    bool alarm = false;
    if (batteryLevel < 10)
        alarm = true;
    if (voltage > 4.2 && current > 1000.0)
        alarm = true;

    if (alarm)
    {
        unsigned long now = millis();
        if (now - alarmBlinkLast >= 500)
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
    Serial.print("[STATUS] BTN=");
    Serial.print(reading == LOW ? "PRESSED" : "RELEASED");
    Serial.print(" | ON_PIN=");
    Serial.print(digitalRead(ON_PIN));
    Serial.print(" | OFF_PIN=");
    Serial.print(digitalRead(OFF_PIN));
    Serial.print(" | ALARM_PIN=");
    Serial.println(digitalRead(ALARM_PIN));

    delay(150);
}

#endif
