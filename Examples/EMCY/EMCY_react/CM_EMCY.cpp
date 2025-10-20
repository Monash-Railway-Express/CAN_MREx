/**
 * CAN MREX Emergency file
 *
 * File:            CM_EMCY.cpp
 * Organisation:    MREX
 * Author:          Chiara Gillam
 * Date Created:    12/09/2025
 * Last Modified:   15/10/2025
 * Version:         1.11.0
 *
 */

#include "driver/twai.h"
#include <Arduino.h>
#include "CM_ObjectDictionary.h"
#include "CM_EMCY.h"

const uint8_t MAX_MINOR_EMCY_COUNT = 5;
uint8_t minorEMCYCount = 0;

void handleEMCY(const twai_message_t& rxMsg, uint8_t nodeID){
  if (rxMsg.data[0] == 0x00) nodeOperatingMode = 0x02;
  if (rxMsg.data[0] == 0x01) minorEMCYCount += 1;
}



void sendEMCY(uint8_t priority, uint8_t nodeID, uint32_t errorCode){
  twai_message_t txMsg;
  txMsg.identifier = 0x080 + nodeID;
  txMsg.data_length_code = 6;
  txMsg.data[0] = priority;
  txMsg.data[1] = nodeID;
  txMsg.data[2] = errorCode & 0xFF;
  txMsg.data[3] = (errorCode >> 8) & 0xFF;
  txMsg.data[4] = (errorCode >> 16) & 0xFF;
  txMsg.data[5] = (errorCode >> 24) & 0xFF;

  if (priority == 0x00) {
    nodeOperatingMode = 0x02;  // Stop system
  }

  if (priority == 0x01) {
    minorEMCYCount += 1;
    if (minorEMCYCount >= MAX_MINOR_EMCY_COUNT) {
      minorEMCYCount = 0;  // Reset to avoid infinite loop
      sendEMCY(0x00, nodeID, 0x00000301);  // Major EMCY
      return;
    }
  }

  if (twai_transmit(&txMsg, pdMS_TO_TICKS(100)) != ESP_OK) {
    if (twai_transmit(&txMsg, pdMS_TO_TICKS(100)) != ESP_OK) {
      Serial.println("EMCY transmission failed twice");
    }
  }
}

