#ifndef LOGIC_H
#define LOGIC_H
#include <Arduino.h>
#include "tempSensor.h"

// Pin map (Wemos D1 mini)
// D7 -> IN1 (GPIO13), D5 -> IN2 (GPIO14), D6 -> ENA (GPIO12)
#define IN1_PIN 13
#define IN2_PIN 14
#define ENA_PIN 12

// Timing and state constants (replace magic numbers)
const unsigned long LOGIC_LOOP_DELAY_MS = 150; // ms delay at end of logic loop

// Motor power rules
const int BASE_POWER_PERCENT = 100;
const float TEMP_DERATE_START = 30.0f;
const float TEMP_STOP = 50.0f;
const float TEMP_STEP = 10.0f;
const int POWER_STEP = 101;

bool motorRunning = false;
int motorPowerPercent = 0;
int motorPwmValue = 0;

void setup_logic()
{
    pinMode(IN1_PIN, OUTPUT);
    pinMode(IN2_PIN, OUTPUT);
    pinMode(ENA_PIN, OUTPUT);

    digitalWrite(IN1_PIN, LOW);
    digitalWrite(IN2_PIN, LOW);
    analogWriteRange(1023);
    analogWriteFreq(1000);
    analogWrite(ENA_PIN, 0);

    Serial.println("[LOGIC] Setup complete");
}

void loop_logic()
{
    int power = BASE_POWER_PERCENT;

    if (temperatureC >= TEMP_STOP)
    {
        power = 0;
    }
    else if (temperatureC > TEMP_DERATE_START)
    {
        int steps = (int)((temperatureC - TEMP_DERATE_START) / TEMP_STEP);
        power = BASE_POWER_PERCENT - (steps * POWER_STEP);
    }

    motorPowerPercent = constrain(power, 0, 100);
    motorRunning = motorPowerPercent > 0;
    motorPwmValue = map(motorPowerPercent, 0, 100, 0, 1023);

    if (motorRunning)
    {
        digitalWrite(IN1_PIN, HIGH);
        digitalWrite(IN2_PIN, LOW);
    }
    else
    {
        digitalWrite(IN1_PIN, LOW);
        digitalWrite(IN2_PIN, LOW);
    }

    analogWrite(ENA_PIN, motorPwmValue);

    // --- Статус логування ---
    Serial.print("[STATUS] Temp=");
    Serial.print(temperatureC, 1);
    Serial.print("C | Power=");
    Serial.print(motorPowerPercent);
    Serial.print("% | PWM=");
    Serial.print(motorPwmValue);
    Serial.print(" | Running=");
    Serial.print(motorRunning ? "YES" : "NO");
    Serial.print(" | IN1=");
    Serial.print(digitalRead(IN1_PIN));
    Serial.print(" | IN2=");
    Serial.print(digitalRead(IN2_PIN));
    Serial.print(" | ENA=");
    Serial.println(motorPwmValue);

    delay(LOGIC_LOOP_DELAY_MS);
}

#endif
