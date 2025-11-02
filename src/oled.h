#ifndef OLED_H
#define OLED_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "powerSensor.h"
#include "tempSensor.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void oled_init()
{
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;)
            ;
    }
    delay(2000);
    display.clearDisplay();

    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.display();
}

void oledPrint()
{
    // update display with latest readings
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println("Power Station");

    display.setCursor(0, 12);
    display.print("Voltage: ");
    display.println(voltage, 2);

    display.print("Current: ");
    display.println(current, 2);

    display.print("Power: ");
    display.println(power, 2);

    display.print("Temperature: ");
    display.println(temperatureC, 2);

    display.display();
}

#endif