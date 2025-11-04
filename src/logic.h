#ifndef LOGIC_H
#define LOGIC_H
#include <Arduino.h>
#include "powerSensor.h"
#include "tempSensor.h"

#define BTN_PIN 14
#define ON_PIN 15
#define OFF_PIN 13
#define ALARM_PIN 12

// Timing and state constants (replace magic numbers)
const unsigned long DEBOUNCE_DELAY_MS = 50;        // ms for button debounce
const unsigned long ALARM_BLINK_INTERVAL_MS = 500; // ms interval for alarm blinking
const unsigned long LOGIC_LOOP_DELAY_MS = 150;     // ms delay at end of logic loop

// Button active/released states (INPUT_PULLUP wiring)
const int BTN_ACTIVE_STATE = LOW;
const int BTN_RELEASED_STATE = HIGH;

// Alarm logic thresholds (extract magic numbers here)
// Alarm logic thresholds (configurable at runtime)
uint8_t alarmBatteryPercent = 10;   // if batteryLevel < this → alarm
float alarmVoltageV = 4.2;         // voltage threshold in volts
float alarmCurrentmA = 1000.0;     // current threshold in milliamps

bool alarm = false;

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
int lastReading = BTN_RELEASED_STATE;
int stableState = BTN_RELEASED_STATE;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = DEBOUNCE_DELAY_MS;

// Допоміжна змінна для виявлення перехідного фронту (HIGH -> LOW)
int prevStableState = HIGH;
// Кнопка "застреленa" після натискання, поки не відпущена
bool buttonArmed = true;

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

            // Переключаємо лише на чистому переході RELEASED -> ACTIVE і тільки якщо кнопка "застрелена" (re-armed)
            if (stableState == BTN_ACTIVE_STATE && prevStableState == BTN_RELEASED_STATE && buttonArmed)
            {
                stationOn = !stationOn;
                buttonArmed = false; // чекаємо відпускання
                Serial.print("[BUTTON] Press detected -> Station state: ");
                Serial.println(stationOn ? "ON" : "OFF");
            }

            // Коли кнопка відпущена (RELEASED) — реармимо її для наступного натискання
            if (stableState == BTN_RELEASED_STATE)
            {
                buttonArmed = true;
            }

            prevStableState = stableState;
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
    alarm = false; // скидання перед перевірками
    if (batteryLevel < alarmBatteryPercent)
        alarm = true;
    if (voltage > alarmVoltageV && current > alarmCurrentmA)
        alarm = true;

    if (alarm)
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
    Serial.print("[STATUS] BTN=");
    Serial.print(reading == LOW ? "PRESSED" : "RELEASED");
    Serial.print(" | ON_PIN=");
    Serial.print(digitalRead(ON_PIN));
    Serial.print(" | OFF_PIN=");
    Serial.print(digitalRead(OFF_PIN));
    Serial.print(" | ALARM_PIN=");
    Serial.println(digitalRead(ALARM_PIN));

    delay(LOGIC_LOOP_DELAY_MS);
}

#endif
