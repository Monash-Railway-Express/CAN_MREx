/**
 * CAN MREX Process Data object file
 *
 * File:            CM_PDO.h
 * Organisation:    MREX
 * Author:          Chiara Gillam
 * Date Created:    18/08/2025
 * Last Modified:   8/09/2025
 * Version:         1.10.1
 *
 */


#ifndef CM_PDO_H
#define CM_PDO_H

#include <Arduino.h>
#include "driver/twai.h"

struct PdoComm {
  uint32_t cob_id;        // sub1
  uint8_t  trans_type;    // sub2 (255 async; 0/1..240 sync types)
  uint16_t inhibit_time;  // sub3 in 100 µs units (CiA); we’ll store ms for simplicity
  uint16_t event_timer;   // sub5 in ms
  bool     enabled;       // derived from cob_id bit31 == 0
};

struct PdoMapEntry {
  uint16_t index;
  uint8_t  subindex;
  uint8_t  len_bits;   // 8,16,32 for byte-aligned
};

struct PdoMap {
  uint8_t count;
  PdoMapEntry e[8];    // up to 8 entries (<= 64 bytes total for CAN FD, but we’ll cap to 8 bytes classic)
};

struct TpdoState {
  uint32_t last_tx_ms;
  uint32_t inhibit_ms;   // derived from inhibit_time
  uint8_t  last_payload[8];
  uint8_t  last_len;
  bool     last_valid;
};

void initDefaultPDOs(uint8_t nodeID);

// Call in loop
void processRPDO(const twai_message_t& rx);
void serviceTPDOs(uint8_t nodeID);  // handles periodic/event-driven sends

// Helpers
bool packTPDO(uint8_t pdoNum, uint8_t* outBytes, uint8_t* outLen);
bool unpackRPDO(uint8_t pdoNum, const uint8_t* data, uint8_t len);

// Optional: expose a simple API to trigger event-driven sends on change
void markTpdoDirty(uint8_t pdoNum);

// Communication setup
void configureTPDO(uint8_t pdoNum, uint32_t cobID, uint8_t transType, uint16_t inhibitMs, uint16_t eventMs);
void configureRPDO(uint8_t pdoNum, uint32_t cobID, uint8_t transType, uint16_t inhibitMs);

// Mapping setup
bool mapTPDO(uint8_t pdoNum, const PdoMapEntry* entries, uint8_t count);
bool mapRPDO(uint8_t pdoNum, const PdoMapEntry* entries, uint8_t count);


#endif