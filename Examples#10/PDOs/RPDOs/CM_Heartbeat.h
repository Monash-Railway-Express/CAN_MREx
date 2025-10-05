/**
 * CAN MREX Heartbeat Header
 *
 * File:            CM_Heartbeat.h
 * Organisation:    MREX
 * Author:          Chiara Gillam
 * Date Created:    12/09/2025
 * Last Modified:   30/09/2025
 * Version:         1.10.2
 */

#ifndef CM_HEARTBEAT_H
#define CM_HEARTBEAT_H

#include <Arduino.h>
#include "driver/twai.h"

#define MAX_NODES 16  // Adjust based on your network size

typedef struct {
  uint8_t hbOperatingMode;
  uint32_t lastHeartbeat;
} nodeHeartbeat;

extern nodeHeartbeat heartbeatTable[MAX_NODES];

void sendHeartbeat(uint8_t nodeID);
void receiveHeartbeat(const twai_message_t& rxMsg);
void checkHeartbeatTimeouts();
void setupHeartbeatConsumer();

#endif
