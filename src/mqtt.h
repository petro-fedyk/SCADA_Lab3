#ifndef MQTT_H
#define MQTT_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "logic.h"
#include "powerSensor.h"
#include "tempSensor.h"

// --- WiFi Settings ---
#define WIFI_SSID "admin"
#define WIFI_PASSWORD "domestos1216"

// --- MQTT Broker IP ---
#define MQTT_SERVER "192.168.1.45" // <-- IP твоєї Ubuntu машини
#define MQTT_PORT 1883

WiFiClient espClient;
PubSubClient client(espClient);

// --- WiFi setup ---
void setup_wifi()
{
    delay(10);
    Serial.println();
    Serial.print("[WIFI] Connecting to ");
    Serial.println(WIFI_SSID);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("[WIFI] Connected!");
    Serial.print("[WIFI] IP: ");
    Serial.println(WiFi.localIP());
}

// --- MQTT reconnect ---
void reconnect_mqtt()
{
    while (!client.connected())
    {
        Serial.print("[MQTT] Attempting connection...");
        if (client.connect("ESP8266_Client"))
        {
            Serial.println("connected!");
        }
        else
        {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" -> retry in 5s");
            delay(5000);
        }
    }
}

// --- MQTT setup ---
void setup_mqtt()
{
    setup_wifi();
    client.setServer(MQTT_SERVER, MQTT_PORT);
    Serial.println("[MQTT] Setup complete");
}

// --- MQTT loop ---
void loop_mqtt()
{
    if (!client.connected())
    {
        reconnect_mqtt();
    }
    client.loop();


    // Build JSON payload and publish to test topic
    const char *topic = "scada/lab3/test";

    bool alarmFlag = false;
    if (batteryLevel < alarmBatteryPercent) alarmFlag = true;
    if (voltage > alarmVoltageV && current > alarmCurrentmA) alarmFlag = true;

    String payload = "{";
    payload += "\"temperature\":" + String(temperatureC, 2) + ",";
    payload += "\"voltage\":" + String(voltage, 2) + ",";
    payload += "\"current\":" + String(current, 2) + ",";
    payload += "\"power\":" + String(power, 2) + ",";
    payload += "\"battery\":" + String(batteryLevel) + ",";
    payload += "\"alarm\":" + String(alarmFlag ? "true" : "false");
    payload += "}";

    client.publish(topic, payload.c_str());

    Serial.print("[MQTT] Published to ");
    Serial.print(topic);
    Serial.print(": ");
    Serial.println(payload);

    delay(5000);
}

#endif
