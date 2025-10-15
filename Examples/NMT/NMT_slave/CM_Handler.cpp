/**
 * CAN MREX Handler file 
 *
 * File:            CM_Handler.cpp
 * Organisation:    MREX
 * Author:          Chiara Gillam
 * Date Created:    6/08/2025
 * Last Modified:   12/09/2025
 * Version:         1.10.2
 *
 */


#include "CM_Handler.h"
#include "driver/twai.h"
#include "CM_ObjectDictionary.h"
#include "CM_SDO.h"
#include "CM_PDO.h"
#include "CM_NMT.h"
#include "CM_EMCY.h"
#include "CM_Heartbeat.h"

void handleCAN(uint8_t nodeID, twai_message_t* pdoMsg) {
  serviceTPDOs(nodeID); // Handles all TPDOs to be sent
  sendHeartbeat(nodeID); //sends Heartbeat periodically
  // checkHeartbeatTimeouts(); // Checks heartbeats to make sure they're not overdue (Heartbeat consumer only)

  // Receive the message
  twai_message_t rxMsg;
  if (pdoMsg == nullptr) {
    if (twai_receive(&rxMsg, pdMS_TO_TICKS(5)) != ESP_OK) return; // Timeout or error
  } else {
    rxMsg = *pdoMsg;
  }

  // Handle the message
  uint32_t canID = rxMsg.identifier;

  if (canID == 0x000) { // NMT commands (always processed)
    handleNMT(rxMsg, nodeID);
    return;
  } 
  else if (canID == 0x081) { // Emergency messages (always processed)
    handleEMCY(rxMsg, nodeID);
    return;
  } 
  else if ((canID >= 0x180 && canID <= 0x57F) && nodeOperatingMode == 0x80) { // RPDOs (only in operational state)
    processRPDO(rxMsg);
    return;
  } 
  else if (canID == 0x600 + nodeID) { // SDOs (processed in pre-op and operational)
    handleSDO(rxMsg, nodeID);
    return;
  } 
  // else if (canID >= 0x700 && canID <= 0x780) { // Heartbeats
  //   receiveHeartbeat(rxMsg);
  //   return;
  // } // For heartbeat consumer only
  else {
    return;
  }
}





