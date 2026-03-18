/**
 * CAN MREX Heartbeat File
 *
 * File:            CM_Heartbeat.cpp
 * Organisation:    MREX
 * Author:          Chiara Gillam
 * Date Created:    12/09/2025
 * Last Modified:   14/03/2026
 * Version:         1.13.0
 */


#include "CM_Heartbeat.h"
#include "CM_ObjectDictionary.h"
#include "CM_EMCY.h"

nodeHeartbeat heartbeatTable[MAX_NODES];
bool heartbeatMonitoringEnabled = true;

static uint32_t lastHeartbeatSendTime = 0;
const uint32_t heartbeatTimeout = 1500;   // 1.5 seconds

// --- Producer Functions ---
void sendHeartbeat(uint8_t nodeID) {
  uint32_t currentMs = millis();
  if (currentMs - lastHeartbeatSendTime >= heartbeatInterval) {
    twai_message_t txMsg;
    txMsg.identifier = 0x700 + nodeID;
    txMsg.data_length_code = 1;
    txMsg.data[0] = nodeOperatingMode;
    txMsg.flags = TWAI_MSG_FLAG_NONE;

    if (twai_transmit(&txMsg, pdMS_TO_TICKS(100)) == ESP_OK) {
      lastHeartbeatSendTime = currentMs;
    }
  }
}

// --- Consumer Functions ---
void receiveHeartbeat(const twai_message_t& rxMsg) {
    if (!heartbeatMonitoringEnabled) return;

    uint8_t nodeIndex = rxMsg.identifier - 0x700;
    if (nodeIndex < MAX_NODES && rxMsg.data_length_code >= 1) {
        heartbeatTable[nodeIndex].hbOperatingMode = rxMsg.data[0];
        heartbeatTable[nodeIndex].lastHeartbeat = millis();
    }
}

void checkHeartbeatTimeouts() {
    if (!heartbeatMonitoringEnabled) return;

    static uint32_t lastCheckTime = 0;
    uint32_t currentMs = millis();

    if (currentMs - lastCheckTime < 1000) return;
    lastCheckTime = currentMs;

    for (uint8_t i = 0; i < MAX_NODES; i++) {
        if (heartbeatTable[i].lastHeartbeat > 0 &&
            currentMs - heartbeatTable[i].lastHeartbeat > heartbeatTimeout) {
            sendEMCY(0x00, i, 0x00000101);
            Serial.print("Error 0x00000101: Heartbeat timeout on node ");
            Serial.println(i);
        }
    }
}

void setupHeartbeatConsumer() {
  for (uint8_t i = 0; i < MAX_NODES; i++) {
    heartbeatTable[i].hbOperatingMode = 0x00;
    heartbeatTable[i].lastHeartbeat = 0;
  }
}

void enableHeartbeatMonitoring(bool enable) {
    heartbeatMonitoringEnabled = enable;
}

bool isHeartbeatMonitoringEnabled() {
    return heartbeatMonitoringEnabled;
}

const nodeHeartbeat* getHeartbeatTable() {
    return heartbeatTable;
}