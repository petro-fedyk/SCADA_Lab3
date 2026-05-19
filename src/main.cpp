#include <Arduino.h>

#include <Wire.h>
#include "oled.h"
#include "logic.h"
#include "mqtt.h"
#include "web.h"
#include "tempSensor.h"

void setup()
{
  Serial.begin(115200);
  const int SDA_PIN = 4; // D2 (GPIO4)
  const int SCL_PIN = 5; // D1 (GPIO5)
  Wire.begin(SDA_PIN, SCL_PIN);
  oled_init();
  ds18b20_init();
  setup_logic();
  setup_mqtt();
  setup_web();
}

void loop()
{
  read_temperature();
  oledPrint();
  loop_logic();
  loop_mqtt();
  web_loop();

  delay(1000);
}
