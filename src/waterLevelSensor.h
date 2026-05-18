#ifndef WATER_LEVEL_SENSOR_H
#define WATER_LEVEL_SENSOR_H

#include <Arduino.h>

// --- Water level sensor (0..100%) ---
// For real sensor on ESP8266 A0 use analogRead.
// Set to 1 to simulate level for lab/demo without hardware.
#ifndef USE_FAKE_WATER_LEVEL
#define USE_FAKE_WATER_LEVEL 0
#endif

const int WATER_SENSOR_PIN = A0;
float water_level = 0.0f;

void water_level_init()
{
    if (!USE_FAKE_WATER_LEVEL)
    {
        pinMode(WATER_SENSOR_PIN, INPUT);
    }
}

void read_water_level()
{
#if USE_FAKE_WATER_LEVEL
    // Smooth random walk between 0..100
    static float level = 50.0f;
    static int direction = 1;

    float step = random(5, 15) / 10.0f; // 0.5..1.4
    level += direction * step;

    if (level >= 100.0f)
    {
        level = 100.0f;
        direction = -1;
    }
    if (level <= 0.0f)
    {
        level = 0.0f;
        direction = 1;
    }

    water_level = level;
#else
    int raw = analogRead(WATER_SENSOR_PIN); // 0..1023
    float percent = (raw / 1023.0f) * 100.0f;
    water_level = constrain(percent, 0.0f, 100.0f);

    // If value looks like normalized 0..1, scale to percent
    if (water_level > 0.0f && water_level <= 1.0f)
    {
        water_level = constrain(water_level * 100.0f, 0.0f, 100.0f);
    }
#endif

    Serial.print("[WATER] Level: ");
    Serial.print(water_level, 1);
    Serial.println(" %");
}

#endif
