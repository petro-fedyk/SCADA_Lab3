# Smart Power Bank Prototype

## Overview
This project represents a prototype of a **smart charging station** (DIY power bank) based on the **ESP8266** microcontroller.  
It monitors voltage, current, and temperature, controls the load through a transistor, and provides both **OLED display output** and **web interface** control.

---

## Features
- 🔋 **INA226 sensor** for measuring voltage, current, and power  
- 🌡️ **DS18B20 temperature sensor** for monitoring battery cell temperature  
- 🖥️ **OLED display (I2C)** for real-time data visualization  
- 🌐 **Web interface** for remote monitoring and control  
- 🔘 **Push button** for local user interaction  
- ⚡ **Transistor-controlled load** to simulate or manage power output  
- 💾 Optional data logging or calibration for further expansion  

---

## Hardware Components
| Component | Description |
|------------|-------------|
| **ESP8266 (NodeMCU / Wemos D1 mini)** | Main controller, Wi-Fi + I/O |
| **INA226** | I2C voltage & current sensor |
| **DS18B20** | 1-Wire temperature sensor |
| **OLED 0.96" (SSD1306)** | I2C display for live readings |
| **NPN/MOSFET transistor** | Load control switch |
| **Push button** | Manual control input |
| **Li-ion cells + protection** | Power source |
| **Resistors, wiring, PCB** | Support components |

---

## System Description
1. **Power Measurement:**  
   INA226 continuously measures bus voltage and current. Data is sent to ESP8266 via I2C.  
2. **Temperature Monitoring:**  
   DS18B20 provides real-time cell temperature to prevent overheating.  
3. **Load Control:**  
   The transistor acts as an electronic switch, controlled by the ESP8266 GPIO.  
4. **User Interface:**  
   - OLED screen displays live voltage, current, power, and temperature.  
   - Push button cycles display modes or toggles the load.  
   - Web interface (via ESP8266 Wi-Fi) allows remote control and visualization of all parameters.  

---

## Software
The firmware is written in **C/C++ using the Arduino framework**.  
Main modules:
- `INA226_WE.h` – current/voltage measurement  
- `OneWire.h`, `DallasTemperature.h` – temperature sensor support  
- `Adafruit_SSD1306.h` – OLED display driver  
- `ESP8266WiFi.h`, `ESP8266WebServer.h` – web interface  
- GPIO control logic for button and transistor  

---

## Operation
1. On power-up, the ESP8266 initializes all sensors and the OLED display.  
2. Measurements are updated periodically (1–2 seconds).  
3. The OLED and web interface show:
   - Bus voltage (V)
   - Current (mA)
   - Power (mW)
   - Temperature (°C)
4. The user can toggle the load using:
   - The **push button** (local)
   - The **web interface** (remote)

---

## Future Improvements
- Data logging to EEPROM or cloud  
- Battery level estimation  
- MQTT/HTTP integration for IoT platforms  
- Improved UI with graphs and status indicators  

---

## License
This project is created for educational purposes as part of a laboratory assignment.  
Feel free to modify and reuse it under the terms of the MIT License.
