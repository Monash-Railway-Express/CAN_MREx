// Pinned to ESP32 core 2.0.11 for compatibility with ESPAsyncWebServer. Future migration to ESP-IDF or updated async stack planned.


#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <ArduinoJson.h>

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

const char* ssid = "TrainLogger";
const char* password = "password123";

void setup() {
  Serial.begin(115200);

  // Start Wi-Fi in Access Point mode
  WiFi.softAP(ssid, password);
  Serial.println("Access Point started");
  Serial.println(WiFi.softAPIP());

  // WebSocket event handler
  ws.onEvent([](AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
                void *arg, uint8_t *data, size_t len) {
    if (type == WS_EVT_CONNECT) {
      Serial.printf("Client connected: %u\n", client->id());
    } else if (type == WS_EVT_DISCONNECT) {
      Serial.printf("Client disconnected: %u\n", client->id());
    } else if (type == WS_EVT_DATA) {
      Serial.printf("Received data from client %u\n", client->id());
      // Optional: echo or parse incoming data
    }
  });

  server.addHandler(&ws);
  server.begin();
}

void loop() {
  // Simulate a CAN frame every second
  static unsigned long lastSend = 0;
  if (millis() - lastSend > 1000) {
    lastSend = millis();

    // Create JSON message
    DynamicJsonDocument doc(128);
    doc["ts"] = millis();
    doc["id"] = "0x123";
    JsonArray data = doc.createNestedArray("data");
    data.add(0x01);
    data.add(0x02);
    data.add(0x03);
    data.add(0x04);

    String json;
    serializeJson(doc, json);

    // Send to all connected clients
    ws.textAll(json);
    Serial.println("Sent: " + json);
  }
}
