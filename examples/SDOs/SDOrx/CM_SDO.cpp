#include <stdint.h>
/**
 * CAN MREX SDOs file 
 *
 * File:            CM_SDO.cpp
 * Organisation:    MREX
 * Author:          Chiara Gillam
 * Date Created:    30/09/2025
 * Last Modified:   15/10/2025
 * Version:         1.11.0
 *
 */

#include "driver/twai.h"
#include <Arduino.h>
#include "CM_Handler.h"
#include "CM_SDO.h"
#include "CM_ObjectDictionary.h"
#include "CM_EMCY.h"


void handleSDO(const twai_message_t& rxMsg, uint8_t nodeID) {
  uint16_t index = rxMsg.data[1] | (rxMsg.data[2] << 8);
  uint8_t subindex = rxMsg.data[3];
  uint8_t cmd = rxMsg.data[0];

  // Prepare response message
  twai_message_t txMsg;
  txMsg.identifier = 0x580 + nodeID;
  txMsg.data_length_code = 8;
  txMsg.flags = TWAI_MSG_FLAG_NONE;
  txMsg.data[1] = rxMsg.data[1];
  txMsg.data[2] = rxMsg.data[2];
  txMsg.data[3] = rxMsg.data[3];
  txMsg.data[4] = 0;
  txMsg.data[5] = 0;
  txMsg.data[6] = 0;
  txMsg.data[7] = 0;

  //lookup OD entry
  ODEntry* entry = findODEntry(index, subindex);
  if (entry == nullptr) {
    Serial.println("Error 0x00000001: OD entry not found");
    sendEMCY(0x01, nodeID, 0x00000001);
    return;
  }


  if (cmd == 0x40) { // --- Read request ---
    // Set correct response command byte based on size
    switch (entry->size) {
      case 1: txMsg.data[0] = 0x4F; break;
      case 2: txMsg.data[0] = 0x4B; break;
      case 4: txMsg.data[0] = 0x43; break;
      default:
        Serial.println("Error 0x00000002: OD entry size unsupported");
        sendEMCY(0x01, nodeID, 0x00000002);
        return;
    }

    memcpy(&txMsg.data[4], entry->dataPtr, entry->size);
  }

  else {  // --- write request ---
    // Determine expected write size from command byte
    uint8_t expectedSize = 0;
    switch(cmd) {
      case 0x2F: expectedSize = 1; break;
      case 0x2B: expectedSize = 2; break;
      case 0x23: expectedSize = 4; break;
      default:
        Serial.println("Error 0x00000003: Unexpected SDO write command");
        sendEMCY(0x01, nodeID, 0x00000003);
        return;
    }

    //Copy into the OD
    if (expectedSize == entry->size) {
      memcpy(entry->dataPtr, &rxMsg.data[4], expectedSize);
      txMsg.data[0] = 0x60; // Write confirmation
    } else {
      Serial.println("Error 0x00000004: SDO size mismatch with OD entry");
      sendEMCY(0x01, nodeID, 0x00000004);
      return;
    }
  }

  // Send the response 
  if (twai_transmit(&txMsg, pdMS_TO_TICKS(10)) != ESP_OK) {
    Serial.println("Error 0x00000005: Failed to transmit SDO response");
    sendEMCY(0x01, nodeID, 0x00000005);
  }
}



void executeSDOWrite(uint8_t nodeID, uint8_t targetNodeID, uint16_t index, uint8_t subindex, size_t size, const void* value) {
  uint8_t sdoBuf[8];
  uint8_t cmd;


  switch (size) {
    case 1: cmd = 0x2F; break;
    case 2: cmd = 0x2B; break;
    case 4: cmd = 0x23; break;
    default:
      Serial.println("Error 0x00000006: Invalid object size in executeSDOWrite");
      sendEMCY(0x01, nodeID, 0x00000006);
      return;
  }

  prepareSDOTransmit(cmd, index, subindex, value, size, sdoBuf);
  transmitSDO(nodeID, targetNodeID, sdoBuf, nullptr);
}

uint32_t executeSDORead(uint8_t nodeID, uint8_t targetNodeID, uint16_t index, uint8_t subindex) {
  uint8_t sdoBuf[8];
  uint8_t cmd = 0x40;
  uint32_t outValue = 0;
  

  prepareSDOTransmit(cmd, index, subindex, nullptr, 0, sdoBuf);
  transmitSDO(nodeID, targetNodeID, sdoBuf, &outValue);
  return outValue;
}

//used to prepare the message being sent over SDO
void prepareSDOTransmit(uint8_t cmd, uint16_t index, uint8_t subindex, const void* value, size_t size, uint8_t* outBuf) {
  outBuf[0] = cmd;
  outBuf[1] = index & 0xFF;
  outBuf[2] = (index >> 8) & 0xFF;
  outBuf[3] = subindex;

  // Copy value into data[4..7]
  memset(&outBuf[4], 0, 4); // Clear padding
  if (value != nullptr && size > 0) {
    memcpy(&outBuf[4], value, size);
  }
}

void transmitSDO(uint8_t nodeID, uint8_t targetNodeID, uint8_t* data, uint32_t* outValue) { 
  // Prepare SDO for transmit
  twai_message_t msg;
  msg.identifier = 0x600 + targetNodeID;
  msg.data_length_code = 8;
  msg.flags = TWAI_MSG_FLAG_NONE;
  memcpy(msg.data, data, 8);

  // Transmit SDO request
  if (twai_transmit(&msg, pdMS_TO_TICKS(10)) != ESP_OK) {
    Serial.println("Error 0x00000007: Failed to transmit SDO request");
    sendEMCY(0x01, nodeID, 0x00000007);
    return;
  }

  waitSDOResponse(outValue, targetNodeID, nodeID);
}

void waitSDOResponse(uint32_t* outValue, uint8_t targetNodeID, uint8_t nodeID){
  // Wait for response
  unsigned long start = millis();
  twai_message_t response;
  while (millis() - start < 200) { // try until timeout and ensure messages received before are handled
    if (twai_receive(&response, pdMS_TO_TICKS(50)) != ESP_OK)
      continue;

    if (response.identifier == 0x580 + targetNodeID) { // Received response
      uint8_t cmd = response.data[0];

      if (cmd == 0x60) return; // SDO Confirmed

      if (cmd == 0x80) {
        Serial.println("Error 0x00000009: SDO Abort received");
        sendEMCY(0x01, nodeID, 0x00000009);
        return;
      }

      if (cmd == 0x4F || cmd == 0x4B || cmd == 0x43) { // SDO Read 1, 2, 4 bytes
        int32_t value = 0;
        switch (cmd) {
          case 0x4F: value = response.data[4]; break;
          case 0x4B: value = response.data[4] | (response.data[5] << 8); break;
          case 0x43: value = response.data[4] | (response.data[5] << 8) |
                            (response.data[6] << 16) | (response.data[7] << 24); break;
        }

        //Return value 
        if (outValue != nullptr) {
          *outValue = value;
        }
        return;
      }

      sendEMCY(0x01, nodeID, 0x0000000A); // Unexpected SDO CMD received in response
      Serial.println("Error 0x0000000A: Unexpected SDO command in response");
      return;



    } else{ // handle messages that aren't the response 
      handleCAN(nodeID, &response);   
    }
  }

  sendEMCY(0x00, nodeID, 0x00000008); // SDO response not received
  Serial.println("Error 0x00000008: SDO response timeout");

}