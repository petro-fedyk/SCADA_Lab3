#include <Arduino.h>

#include <Wire.h>
#include "waterLevelSensor.h"
#include "oled.h"
#include "logic.h"
#include "mqtt.h"
#include "web.h"

void setup()
{
  Serial.begin(115200);
  Wire.begin();
  oled_init();
  water_level_init();
  setup_logic();
  setup_mqtt();
  setup_web();
}

void loop()
{
  read_water_level();
  oledPrint();
  loop_logic();
  loop_mqtt();
  web_loop();

  delay(1000);
}
