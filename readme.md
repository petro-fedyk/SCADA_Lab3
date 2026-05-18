# Industrial Pump Station (ESP8266)

## Overview
This project represents a **pump station monitoring node** based on the **ESP8266** microcontroller.  
It reads the **water level** (0–100%), controls a **pump/relay**, and publishes telemetry to a local **MQTT broker**. A simple **OLED** and **web UI** visualize the state.

---

## Features
- � **Water level sensor** (A0 or simulated) with 0–100% scaling
- ⚙️ **Pump relay control** with manual + auto modes
- 🖥️ **OLED display (I2C)** for local status
- 🌐 **Web interface** for monitoring and control
- � **MQTT telemetry** in JSON format
- 🔘 **Push button** for manual override

---

## Hardware Components
| Component | Description |
|------------|-------------|
| **ESP8266 (Wemos D1 mini)** | Main controller, Wi-Fi + I/O |
| **Water level sensor (T1592)** | Analog level measurement (0–100%) |
| **Relay module** | Pump control output |
| **OLED 0.96" (SSD1306)** | I2C display for live readings |
| **Push button** | Manual override input |
| **Resistors, wiring, PCB** | Support components |

---

## System Description
1. **Water Level Monitoring:**  
   Analog input is scaled to **0–100%**. A simulated mode is available for labs without hardware.  
2. **Pump Control:**  
   Manual mode toggles the relay directly. Auto mode enables hysteresis control (20%/80%).  
3. **Alarm Indication:**  
   `alarm_indc` is set to **1** for low level and **2** for high level.  
4. **User Interface:**  
   - OLED shows water level, pump state, auto mode, and alarm code.  
   - Web UI provides live status and toggles for pump/auto.  

---

## Software
The firmware is written in **C/C++ using the Arduino framework**.  
Main modules:
- `waterLevelSensor.h` – water level read/simulation  
- `logic.h` – pump control + alarms  
- `mqtt.h` – JSON telemetry publishing  
- `oled.h` – local display  
- `web.h` – web UI endpoints  

---

## Operation
1. On power-up, the ESP8266 initializes all sensors and the OLED display.  
2. Measurements are updated periodically (1–2 seconds).  
3. The OLED and web interface show:
   - Water level (%)
   - Pump state (ON/OFF)
   - Auto mode (ON/OFF)
   - Alarm code (0/1/2)
4. The user can control the pump using:
   - The **push button** (local)
   - The **web interface** (remote)
   - **MQTT commands** on `scada/lab3/cmd`

### MQTT Payload
Telemetry topic: `scada/lab3/telemetry`

```json
{
  "water_level": 56.4,
  "pump": true,
  "auto_pump": false,
  "alarm_indc": 0
}
```

---

## Future Improvements
- Real sensor calibration for T1592  
- MQTT dashboards (Grafana/Node-RED)  
- Improved UI with graphs and status indicators  

---

## License
This project is created for educational purposes as part of a laboratory assignment.  
Feel free to modify and reuse it under the terms of the MIT License.
