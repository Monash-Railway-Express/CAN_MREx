/**
 * CAN MREX Emergency file
 *
 * File:            CM_EMCY.cpp
 * Organisation:    MREX
 * Author:          Chiara Gillam
 * Date Created:    12/09/2025
 * Last Modified:   14/03/2026
 * Version:         1.13.0
 *
 */

// --- Includes ---
#include <driver/twai.h>
#include <Arduino.h>
#include "CM_ObjectDictionary.h"
#include "CM_EMCY.h"

//Setup max minor emergencies
// const uint8_t MAX_MINOR_EMCY_COUNT = 5;
// uint8_t minorEMCYCount = 0;

// --- Minor / Major Emergency buffer sizes ---
#define MINOR_BUFFER_SIZE 64
#define MAJOR_BUFFER_SIZE 32

// --- Setup structs for emergency bufffers ---
typedef struct {
    uint8_t EMCYNode;
    uint32_t errorCode;
} EMCYStruct;

EMCYStruct minorStruct[MINOR_BUFFER_SIZE];
EMCYStruct majorStruct[MAJOR_BUFFER_SIZE];

volatile uint8_t majorHead = 0; 
volatile uint8_t majorTail = 0;

volatile uint8_t minorHead = 0;
volatile uint8_t minorTail = 0;

volatile uint32_t updateCounterMajor = 0;
volatile uint32_t updateCounterMinor = 0;
volatile uint32_t updateCounterMajorLast = 0;
volatile uint32_t updateCounterMinorLast = 0;


// --- Check if emergencies have updated
bool checkMinorEMCY() {
    return updateCounterMinor != updateCounterMinorLast;
}
bool checkMajorEMCY() {
    return updateCounterMajor != updateCounterMajorLast;
}

// --- Push minor code ---
void pushMinor(uint8_t node, uint32_t code) {
    EMCYStruct p;
    p.EMCYNode = node;
    p.errorCode = code;

    minorStruct[minorHead] = p;

    uint8_t next = (minorHead + 1) % MINOR_BUFFER_SIZE;

    if (next == minorTail) {
        // buffer full → drop oldest
        minorTail = (minorTail + 1) % MINOR_BUFFER_SIZE;
    }

    minorHead = next;
    updateCounterMinor++;
}

// --- Push major code ---
void pushMajor(uint8_t node, uint32_t code) {
    EMCYStruct p;
    p.EMCYNode = node;
    p.errorCode = code;

    majorStruct[majorHead] = p;

    uint8_t next = (majorHead + 1) % MAJOR_BUFFER_SIZE;

    if (next == majorTail) {
        // buffer full → drop oldest
        majorTail = (majorTail + 1) % MAJOR_BUFFER_SIZE;
    }

    majorHead = next;
    updateCounterMajor++;
}

bool getMinorByIndex(uint8_t index, uint8_t *node, uint32_t *code) {
    uint8_t count = (minorHead >= minorTail)
                    ? (minorHead - minorTail)
                    : (MINOR_BUFFER_SIZE - minorTail + minorHead);

    if (index >= count) {
        return false; // out of range
    }

    // newest entry is at (minorHead - 1)
    uint8_t newest = (minorHead + MINOR_BUFFER_SIZE - 1) % MINOR_BUFFER_SIZE;

    // walk backwards for each index
    uint8_t physical = (newest + MINOR_BUFFER_SIZE - index) % MINOR_BUFFER_SIZE;

    *node = minorStruct[physical].EMCYNode;
    *code = minorStruct[physical].errorCode;

    return true;
}

bool getMajorByIndex(uint8_t index, uint8_t *node, uint32_t *code) {
    uint8_t count = (majorHead >= majorTail)
                    ? (majorHead - majorTail)
                    : (MAJOR_BUFFER_SIZE - majorTail + majorHead);

    if (index >= count) {
        return false; // out of range
    }

    // newest entry is at (majorHead - 1)
    uint8_t newest = (majorHead + MAJOR_BUFFER_SIZE - 1) % MAJOR_BUFFER_SIZE;

    // walk backwards for each index
    uint8_t physical = (newest + MAJOR_BUFFER_SIZE - index) % MAJOR_BUFFER_SIZE;

    *node = majorStruct[physical].EMCYNode;
    *code = majorStruct[physical].errorCode;

    return true;
}

// --- Handle an emergency when recieved ---
void handleEMCY(const twai_message_t& rxMsg, uint8_t nodeID){
  // if (rxMsg.data[0] == 0x01) minorEMCYCount += 1;

  // Extract 32‑bit error code
  uint32_t errorCode;
  memcpy(&errorCode, &rxMsg.data[2], sizeof(uint32_t));

  uint8_t priority = rxMsg.data[0];

  if (priority == 0x00) {
      // Major EMCY received
      pushMajor(nodeID, errorCode);
      nodeOperatingMode = 0x02;   // Stop system
  }
  else if (priority == 0x01) {
      // Minor EMCY received
      pushMinor(nodeID, errorCode);
  }

}

// --- Send emergency function --- 
void sendEMCY(uint8_t priority, uint8_t nodeID, uint32_t errorCode){
  // Log it first
  if (priority == 0x00) {
      pushMajor(nodeID, errorCode);
  } else if (priority == 0x01) {
      pushMinor(nodeID, errorCode);
  }


  twai_message_t txMsg;
  txMsg.identifier = 0x080 + nodeID;
  txMsg.data_length_code = 6;
  txMsg.data[0] = priority;
  txMsg.data[1] = nodeID;
  memcpy(&txMsg.data[2], &errorCode, sizeof(uint32_t));


  if (priority == 0x00) {
    nodeOperatingMode = 0x02;  // Stop system
  }

  if (priority == 0x01) {
    // minorEMCYCount += 1;
    // if (minorEMCYCount >= MAX_MINOR_EMCY_COUNT) {
    //   minorEMCYCount = 0;  // Reset to avoid infinite loop
    //   sendEMCY(0x00, nodeID, 0x00000301);  // Major EMCY
    //   return;
    // }
  }

  if (twai_transmit(&txMsg, pdMS_TO_TICKS(100)) != ESP_OK) {
    if (twai_transmit(&txMsg, pdMS_TO_TICKS(100)) != ESP_OK) {
      Serial.println("EMCY transmission failed twice");
    }
  }
}

