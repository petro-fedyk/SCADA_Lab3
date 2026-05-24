// Simple web UI for SCADA_Lab3
#ifndef WEB_H
#define WEB_H

#include <Arduino.h>
#include <ESP8266WebServer.h>
#include "indexHtml.h"
#include "logic.h"
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
    js += "\"ph\":" + String(waterPh, 2) + ",";
    js += "\"clarity\":" + String(waterClarity, 1) + ",";
    js += "\"pumpPower\":" + String(motorSpeedPercent) + ",";
    js += "\"pumpOn\":" + String(relayOn ? "true" : "false") + ",";
    js += "\"redLed\":" + String(redLedOn ? "true" : "false") + ",";
    js += "\"yellowLed\":" + String(yellowLedOn ? "true" : "false") + "";
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

void setup_web()
{
    server.on("/", HTTP_GET, handleRoot);
    server.on("/status", HTTP_GET, handleStatus);
    server.on("/events", HTTP_GET, handleEvents);
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
            js += "\"ph\":" + String(waterPh, 2) + ",";
            js += "\"clarity\":" + String(waterClarity, 1) + ",";
            js += "\"pumpPower\":" + String(motorSpeedPercent) + ",";
            js += "\"pumpOn\":" + String(relayOn ? "true" : "false") + ",";
            js += "\"redLed\":" + String(redLedOn ? "true" : "false") + ",";
            js += "\"yellowLed\":" + String(yellowLedOn ? "true" : "false") + "";
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
