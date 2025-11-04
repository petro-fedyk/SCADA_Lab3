// Simple web UI for SCADA_Lab3
#ifndef WEB_H
#define WEB_H

#include <Arduino.h>
#include <ESP8266WebServer.h>
#include "indexHtml.h"
#include "logic.h"
#include "powerSensor.h"
#include "tempSensor.h"

ESP8266WebServer server(80);
// Single SSE client support
WiFiClient sseClient;
bool sseClientActive = false;
unsigned long lastSseMs = 0;
const unsigned long SSE_INTERVAL_MS = 1000;

void handleRoot()
{
    server.send_P(200, "text/html", index_html);
}

// Return JSON with current status
void handleStatus()
{
    String js = "{";
    js += "\"temperature\":" + String(temperatureC, 2) + ",";
    js += "\"voltage\":" + String(voltage, 2) + ",";
    js += "\"current\":" + String(current, 2) + ",";
    js += "\"power\":" + String(power, 2) + ",";
    js += "\"battery\":" + String(batteryLevel) + ",";
    js += "\"stationOn\":" + String(stationOn ? "true" : "false") + ",";
    js += "\"onLed\":" + String(digitalRead(ON_PIN) ? "true" : "false") + ",";
    js += "\"offLed\":" + String(digitalRead(OFF_PIN) ? "true" : "false") + ",";
    js += "\"alarmLed\":" + String(digitalRead(ALARM_PIN) ? "true" : "false") + ",";
    js += "\"thresholds\":{\"battery\":" + String(alarmBatteryPercent) + ",\"voltage\":" + String(alarmVoltageV,2) + ",\"current\":" + String(alarmCurrentmA,1) + "}";
    js += "}";

    server.send(200, "application/json", js);
}

// Server-Sent Events endpoint (simple single-client)
void handleEvents()
{
    WiFiClient client = server.client();
    // send headers for SSE
    client.print("HTTP/1.1 200 OK\r\n");
    client.print("Content-Type: text/event-stream\r\n");
    client.print("Cache-Control: no-cache\r\n");
    client.print("Connection: keep-alive\r\n\r\n");
    client.print("retry: 2000\n\n");
    client.flush();
    sseClient = client; // copy client for later writes
    sseClientActive = true;
}

// Toggle station on/off
void handleToggle()
{
    stationOn = !stationOn;
    server.send(200, "text/plain", stationOn ? "ON" : "OFF");
}

// Set thresholds via POST (form data: battery, voltage, current)
void handleSettings()
{
    if (server.method() != HTTP_POST)
    {
        server.send(405, "text/plain", "Method Not Allowed");
        return;
    }

    if (server.hasArg("battery"))
    {
        int b = server.arg("battery").toInt();
        if (b >= 0 && b <= 100)
            alarmBatteryPercent = (uint8_t)b;
    }
    if (server.hasArg("voltage"))
    {
        float v = server.arg("voltage").toFloat();
        alarmVoltageV = v;
    }
    if (server.hasArg("current"))
    {
        float c = server.arg("current").toFloat();
        alarmCurrentmA = c;
    }

    server.send(200, "text/plain", "OK");
}

void setup_web()
{
    server.on("/", HTTP_GET, handleRoot);
    server.on("/status", HTTP_GET, handleStatus);
    server.on("/events", HTTP_GET, handleEvents);
    server.on("/toggle", HTTP_GET, handleToggle);
    server.on("/settings", HTTP_POST, handleSettings);
    server.begin();
    Serial.println("[WEB] Server started");
}

void web_loop()
{
    server.handleClient();

    // SSE: send periodic updates if client connected
    if (sseClientActive)
    {
        if (!sseClient || !sseClient.connected())
        {
            sseClientActive = false;
            return;
        }

        unsigned long now = millis();
        if (now - lastSseMs >= SSE_INTERVAL_MS)
        {
            lastSseMs = now;
            // build JSON same as handleStatus
            String js = "{";
            js += "\"temperature\":" + String(temperatureC, 2) + ",";
            js += "\"voltage\":" + String(voltage, 2) + ",";
            js += "\"current\":" + String(current, 2) + ",";
            js += "\"power\":" + String(power, 2) + ",";
            js += "\"battery\":" + String(batteryLevel) + ",";
            js += "\"stationOn\":" + String(stationOn ? "true" : "false") + ",";
            js += "\"onLed\":" + String(digitalRead(ON_PIN) ? "true" : "false") + ",";
            js += "\"offLed\":" + String(digitalRead(OFF_PIN) ? "true" : "false") + ",";
            js += "\"alarmLed\":" + String(digitalRead(ALARM_PIN) ? "true" : "false") + ",";
            js += "\"thresholds\":{\"battery\":" + String(alarmBatteryPercent) + ",\"voltage\":" + String(alarmVoltageV,2) + ",\"current\":" + String(alarmCurrentmA,1) + "}";
            js += "}";

            // send as SSE data: line starting with "data: " and ending with double newline
            sseClient.print("data: ");
            sseClient.print(js);
            sseClient.print("\n\n");
            sseClient.flush();
        }
    }
}

#endif
