#include <Arduino.h>

#include <Wire.h>
#include "powerSensor.h"
#include "oled.h"
#include "tempSensor.h"
#include "logic.h"
#include "mqtt.h"
#include "web.h"

void setup()
{
  Serial.begin(115200);
  ina226_init(I2C_ADDRESS);
  Wire.begin();
  oled_init();
  ds18b20_init();
  setup_logic();
  setup_mqtt();
  setup_web();
}

void loop()
{
  ina226_read();
  oledPrint();
  read_temperature();
  loop_logic();
  loop_mqtt();
  web_loop();

  delay(1000);
}
