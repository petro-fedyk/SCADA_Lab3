#ifndef MQTT_H
#define MQTT_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "logic.h"
#include "waterLevelSensor.h"

// --- WiFi Settings ---
#define WIFI_SSID "Redmi Note 12 Pro"
#define WIFI_PASSWORD "12032006"

// #define WIFI_SSID "admin"
// #define WIFI_PASSWORD "domestos1216"

// --- MQTT Broker IP ---
#define MQTT_SERVER "192.168.71.216" // <-- IP твоєї Ubuntu машини
#define MQTT_PORT 1883

WiFiClient espClient;
PubSubClient client(espClient);
void mqtt_callback(char *topic, byte *payload, unsigned int length);

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
    client.subscribe("scada/lab3/cmd");
    Serial.println("[MQTT] Subscribed to scada/lab3/cmd");
}

// --- MQTT setup ---
void setup_mqtt()
{
    setup_wifi();
    client.setServer(MQTT_SERVER, MQTT_PORT);
    client.setCallback(mqtt_callback);

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
    const char *topic = "scada/lab3/telemetry";

    String payload = "{";
    payload += "\"water_level\":" + String(water_level, 1) + ",";
    payload += "\"pump\":" + String(pumpOn ? "true" : "false") + ",";
    payload += "\"auto_pump\":" + String(auto_pump ? "true" : "false") + ",";
    payload += "\"alarm_indc\":" + String(alarm_indc);
    payload += "}";

    client.publish(topic, payload.c_str());

    Serial.print("[MQTT] Published to ");
    Serial.print(topic);
    Serial.print(": ");
    Serial.println(payload);

    delay(1000);
}

void mqtt_callback(char *topic, byte *payload, unsigned int length)
{
    String msg;
    for (unsigned int i = 0; i < length; i++)
        msg += (char)payload[i];

    Serial.print("[MQTT] Message arrived: ");
    Serial.println(msg);

    // Примітивний JSON parse (достатньо для лаби)
    bool pumpCmdPresent = false;

    if (msg.indexOf("\"auto_pump\":true") >= 0)
    {
        auto_pump = true;
        Serial.println("[MQTT] auto_pump ON");
    }
    else if (msg.indexOf("\"auto_pump\":false") >= 0)
    {
        auto_pump = false;
        Serial.println("[MQTT] auto_pump OFF");
    }

    if (msg.indexOf("\"pump\":true") >= 0)
    {
        pumpCmdPresent = true;
        if (auto_pump)
            auto_pump = false; // manual override
        pumpOn = true;
        Serial.println("[MQTT] Pump turned ON");
    }
    else if (msg.indexOf("\"pump\":false") >= 0)
    {
        pumpCmdPresent = true;
        if (auto_pump)
            auto_pump = false; // manual override
        pumpOn = false;
        Serial.println("[MQTT] Pump turned OFF");
    }

    if (!pumpCmdPresent && msg.indexOf("\"pump\"") >= 0)
    {
        Serial.println("[MQTT] Pump command present but not parsed");
    }
}

#endif
