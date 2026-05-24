#ifndef OLED_H
#define OLED_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "tempSensor.h"
#include "logic.h"

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

void drawPowerBar(int percent)
{
    int barMargin = 2;
    int barHeight = 10;
    int barX = barMargin;
    int barW = SCREEN_WIDTH - barMargin * 2;
    int barY = SCREEN_HEIGHT - barHeight - 2;

    display.drawRect(barX, barY, barW, barHeight, WHITE);
    int fillW = map(percent, 0, 100, 0, barW - 2);
    if (fillW > 0)
    {
        display.fillRect(barX + 1, barY + 1, fillW, barHeight - 2, WHITE);
    }
}

void drawMotorAnimation(int x, int y)
{
    const int radius = 8;
    uint8_t frame = (millis() / 250) % 4;

    display.drawCircle(x, y, radius, WHITE);

    if (frame == 0)
    {
        display.drawLine(x, y - radius, x, y + radius, WHITE);
        display.drawLine(x - radius, y, x + radius, y, WHITE);
    }
    else if (frame == 1)
    {
        display.drawLine(x - radius, y - radius, x + radius, y + radius, WHITE);
        display.drawLine(x - radius, y + radius, x + radius, y - radius, WHITE);
    }
    else if (frame == 2)
    {
        display.drawLine(x, y - radius, x, y + radius, WHITE);
        display.drawLine(x - radius, y, x + radius, y, WHITE);
        display.drawCircle(x, y, 2, WHITE);
    }
    else
    {
        display.drawLine(x - radius, y - radius, x + radius, y + radius, WHITE);
        display.drawLine(x - radius, y + radius, x + radius, y - radius, WHITE);
        display.drawCircle(x, y, 2, WHITE);
    }
}

void oledPrint()
{
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println("Water Clarity");

    display.setCursor(0, 12);
    display.print("Temp: ");
    display.print(temperatureC, 1);
    display.println(" C");

    display.print("pH: ");
    display.println(waterPh, 2);

    display.print("Clarity: ");
    display.print(waterClarity, 0);
    display.println(" %");

    display.setCursor(0, 42);
    display.print("Pump: ");
    display.print(relayOn ? "ON" : "OFF");

    display.setCursor(74, 42);
    display.print("R/Y");
    if (redLedOn)
        display.fillCircle(92, 46, 3, WHITE);
    if (yellowLedOn)
        display.drawCircle(104, 46, 3, WHITE);

    drawPowerBar(powerPercent);
    drawMotorAnimation(112, 20);

    display.display();
}

#endif