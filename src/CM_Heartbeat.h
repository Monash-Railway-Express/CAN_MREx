/**
 * CAN MREX Heartbeat Header
 *
 * File:            CM_Heartbeat.h
 * Organisation:    MREX
 * Author:          Chiara Gillam
 * Date Created:    12/09/2025
 * Last Modified:   10/03/2026
 * Version:         1.12.3
 */

#ifndef CM_HEARTBEAT_H
#define CM_HEARTBEAT_H

#include <Arduino.h>
#include <driver/twai.h>

#define MAX_NODES 16  // Adjust based on your network size

void enableHeartbeatMonitoring(bool enable);
bool isHeartbeatMonitoringEnabled();

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
