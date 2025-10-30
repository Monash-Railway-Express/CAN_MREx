/**
 * CAN MREX Process Data object file
 *
 * File:            CM_PDO.cpp
 * Organisation:    MREX
 * Author:          Chiara Gillam
 * Date Created:    18/08/2025
 * Last Modified:   15/10/2025
 * Version:         1.11.0
 *
 */

#include "CM_PDO.h"
#include "CM_ObjectDictionary.h"  // for findODEntry
#include <string.h>
#include "CM_EMCY.h"

// Initialise all structs and variables
static PdoComm rpdoComm[4];
static PdoMap  rpdoMap[4];

static PdoComm tpdoComm[4];
static PdoMap  tpdoMap[4];

static TpdoState tpdoState[4];
static bool tpdoDirty[4];

// Sets communication parameters for a PDO (COB-ID, transmission type, timers, enable flag)
static void setComm(PdoComm& c, uint32_t cob, uint8_t ttype, uint16_t inhibit_ms, uint16_t evt_ms) {
  c.cob_id = cob;
  c.trans_type = ttype;
  c.inhibit_time = inhibit_ms;  // stored in ms for runtime simplicity
  c.event_timer = evt_ms;
  c.enabled = ((cob & 0x80000000u) == 0);
}

// Initializes all TPDOs and RPDOs as disabled and clears runtime state
void initDefaultPDOs(uint8_t nodeID) {
  for (int i = 0; i < 4; i++) {
    setComm(tpdoComm[i], 0x80000000u | (0x180 + (i * 0x100) + nodeID), 255, 0, 0); // disabled by bit31
    setComm(rpdoComm[i], 0x80000000u | (0x200 + (i * 0x100) + nodeID), 255, 0, 0);
  }

  memset(tpdoState, 0, sizeof(tpdoState));
  memset(tpdoDirty, 0, sizeof(tpdoDirty));
}

// Reads a mapped variable from the object dictionary and packs it into a TPDO payload
static bool readMapped(const PdoMapEntry& me, uint8_t* out, uint8_t& offsetBytes) {
  ODEntry* od = findODEntry(me.index, me.subindex);
  if (!od || (me.len_bits % 8) != 0) return false;
  uint8_t n = me.len_bits / 8;
  if (offsetBytes + n > 8) return false; // classic CAN
  // CANopen uses little-endian for basic types in mapping
  memcpy(out + offsetBytes, od->dataPtr, n);
  offsetBytes += n;
  return true;
}

// Writes a received RPDO value into the corresponding object dictionary entry
static bool writeMapped(const PdoMapEntry& me, const uint8_t* in, uint8_t& offsetBytes) {
  ODEntry* od = findODEntry(me.index, me.subindex);
  if (!od || (me.len_bits % 8) != 0) return false;
  uint8_t n = me.len_bits / 8;
  if (offsetBytes + n > 8) return false;
  // Basic safety: size must match
  if (od->size != n) return false;
  memcpy(od->dataPtr, in + offsetBytes, n);
  offsetBytes += n;
  return true;
}

// Packs all mapped entries of a TPDO into a CAN payload buffer
bool packTPDO(uint8_t nodeID, uint8_t pdoNum, uint8_t* outBytes, uint8_t* outLen) {
  if (pdoNum >= 4) return false;
  if (!tpdoComm[pdoNum].enabled) return false;
  uint8_t off = 0;
  for (uint8_t i = 0; i < tpdoMap[pdoNum].count; i++) {
    if (!readMapped(tpdoMap[pdoNum].e[i], outBytes, off)) {
      sendEMCY(0x01, nodeID, 0x00000401); // TPDO read mapping failed
      return false;
    }
  }
  *outLen = off;
  return true;
}

// Unpacks an RPDO payload and writes its values into mapped object dictionary entries
bool unpackRPDO(uint8_t nodeID, uint8_t pdoNum, const uint8_t* data, uint8_t len) {
  if (pdoNum >= 4) return false;
  if (!rpdoComm[pdoNum].enabled) return false;
  // Quick length check: sum of bytes must match DLC
  uint8_t needed = 0;
  for (uint8_t i = 0; i < rpdoMap[pdoNum].count; i++) needed += rpdoMap[pdoNum].e[i].len_bits / 8;
  if (needed != len) return false;
  uint8_t off = 0;
  for (uint8_t i = 0; i < rpdoMap[pdoNum].count; i++) {
    if (!writeMapped(rpdoMap[pdoNum].e[i], data, off)) {
      sendEMCY(0x01, nodeID, 0x00000402); // RPDO write mapping failed
      return false;
    }
  }
  return true;
}

// Processes an incoming RPDO message by matching its COB-ID and unpacking its data
void processRPDO(const twai_message_t& rx, uint8_t nodeID) {
  for (uint8_t i = 0; i < 4; i++) {
    if (rpdoComm[i].enabled && rx.identifier == (rpdoComm[i].cob_id & 0x7FF)) {
      if (!unpackRPDO(nodeID, i, rx.data, rx.data_length_code)) {
        sendEMCY(0x01, nodeID, 0x00000404); // RPDO unpack failed
      }
      return;
    }
  }
}

// Marks a TPDO as dirty, triggering event-driven transmission on next service cycle
void markTpdoDirty(uint8_t pdoNum) {
  if (pdoNum < 4) tpdoDirty[pdoNum] = true;
}

// Services all TPDOs: checks timers, dirty flags, inhibits, and transmits if due
void serviceTPDOs(uint8_t nodeID) {
  uint32_t now = millis();

  for (uint8_t i = 0; i < 4; i++) {
    if (!tpdoComm[i].enabled) continue;

    bool due = false;

    // Event timer
    if (tpdoComm[i].event_timer > 0) {
      if (now - tpdoState[i].last_tx_ms >= tpdoComm[i].event_timer) due = true;
    }

    // Event-driven (dirty flag)
    if (tpdoDirty[i]) due = true;

    if (!due) continue;

    // Inhibit time check
    if (tpdoComm[i].inhibit_time > 0) {
      if (now - tpdoState[i].last_tx_ms < tpdoComm[i].inhibit_time) continue;
    }

    uint8_t payload[8] = {0};
    uint8_t len = 0;
    if (!packTPDO(nodeID, i, payload, &len)) continue;

    // Suppress unchanged payload (optional)
    if (tpdoState[i].last_valid && tpdoState[i].last_len == len &&
        memcmp(tpdoState[i].last_payload, payload, len) == 0) {
      tpdoDirty[i] = false;
      continue;
    }

    twai_message_t tx{};
    tx.identifier = tpdoComm[i].cob_id & 0x7FF;
    tx.data_length_code = len;
    tx.flags = TWAI_MSG_FLAG_NONE;
    memcpy(tx.data, payload, len);

    if (twai_transmit(&tx, pdMS_TO_TICKS(10)) == ESP_OK) {
      tpdoState[i].last_tx_ms = now;
      tpdoState[i].last_len = len;
      memcpy(tpdoState[i].last_payload, payload, len);
      tpdoState[i].last_valid = true;
      tpdoDirty[i] = false;
    } else {
      sendEMCY(0x01, nodeID, 0x00000403); // TPDO transmission failed
    }
  }
}

// Configures communication parameters for a TPDO channel
void configureTPDO(uint8_t pdoNum, uint32_t cobID, uint8_t transType, uint16_t inhibitMs, uint16_t eventMs) {
  if (pdoNum < 4) {
    setComm(tpdoComm[pdoNum], cobID, transType, inhibitMs, eventMs);
  }
}

// Configures communication parameters for an RPDO channel
void configureRPDO(uint8_t pdoNum, uint32_t cobID, uint8_t transType, uint16_t inhibitMs) {
  if (pdoNum < 4) {
    setComm(rpdoComm[pdoNum], cobID, transType, inhibitMs, 0); // RPDOs don’t
  }
}

// Maps object dictionary entries to a TPDO channel
bool mapTPDO(uint8_t pdoNum, const PdoMapEntry* entries, uint8_t count) {
  if (pdoNum >= 4 || count > 8) return false;
  tpdoMap[pdoNum].count = count;
  memcpy(tpdoMap[pdoNum].e, entries, count * sizeof(PdoMapEntry));
  return true;
}

// Maps object dictionary entries to an RPDO channel
bool mapRPDO(uint8_t pdoNum, const PdoMapEntry* entries, uint8_t count) {
  if (pdoNum >= 4 || count > 8) return false;
  rpdoMap[pdoNum].count = count;
  memcpy(rpdoMap[pdoNum].e, entries, count * sizeof(PdoMapEntry));
  return true;
}

