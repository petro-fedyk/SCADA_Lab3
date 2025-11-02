#include <Arduino.h>

#include <Wire.h>
#include "powerSensor.h"
#include "oled.h"
#include "tempSensor.h"
#include "logic.h"

void setup()
{
  Serial.begin(115200);
  ina226_init(I2C_ADDRESS);
  Wire.begin();
  oled_init();
  ds18b20_init();
  setup_logic();
}

void loop()
{
  ina226_read();
  oledPrint();
  read_temperature();
  loop_logic();

  delay(1000);
}
