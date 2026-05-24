#ifndef MQTT_H
#define MQTT_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "logic.h"
#include "tempSensor.h"

// --- WiFi Settings ---
// #define WIFI_SSID "Redmi Note 12 Pro"
// #define WIFI_PASSWORD "12032006"

#define WIFI_SSID "admin"
#define WIFI_PASSWORD "domestos1216"

// --- MQTT Broker IP ---
#define MQTT_SERVER "192.168.31.114"
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
    payload += "\"temperature\":" + String(temperatureC, 2) + ",";
    payload += "\"ph\":" + String(waterPh, 2) + ",";
    payload += "\"clarity\":" + String(waterClarity, 1) + ",";
    payload += "\"pumpOn\":" + String(relayOn ? "true" : "false") + ",";
    payload += "\"pumpPower\":" + String(motorSpeedPercent);
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

    // Commands are optional for this lab variant
}

#endif
