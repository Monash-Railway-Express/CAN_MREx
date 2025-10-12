/**
 * CAN MREX Emergency file
 *
 * File:            CM_EMCY.cpp
 * Organisation:    MREX
 * Author:          Chiara Gillam
 * Date Created:    12/09/2025
 * Last Modified:   13/09/2025
 * Version:         1.10.1
 *
 */

#include "driver/twai.h"
#include <Arduino.h>
#include "CM_ObjectDictionary.h"
#include "CM_EMCY.h"


void handleEMCY(const twai_message_t& rxMsg, uint8_t nodeID){
  if (rxMsg.data[0] == 0x00) nodeOperatingMode = 0x02;
  // add in buffer for minor emergencies
}



void sendEMCY(uint8_t priority, uint8_t nodeID, uint32_t errorCode){
  twai_message_t txMsg;
  txMsg.identifier = 0x080 + nodeID;         // EMCY COB-ID per CANopen
  txMsg.data_length_code = 6;
  txMsg.data[0] = priority;
  txMsg.data[1] = nodeID;

  // Copy 32-bit errorCode into data[2..5], little-endian
  txMsg.data[2] = errorCode & 0xFF;
  txMsg.data[3] = (errorCode >> 8) & 0xFF;
  txMsg.data[4] = (errorCode >> 16) & 0xFF;
  txMsg.data[5] = (errorCode >> 24) & 0xFF;

  // Transmit SDO request
  if (twai_transmit(&txMsg, pdMS_TO_TICKS(100)) != ESP_OK) {
    // Try once more
    if (twai_transmit(&txMsg, pdMS_TO_TICKS(100)) != ESP_OK) {
      Serial.println("EMCY transmission failed twice");
      return;
    }
  }
}
