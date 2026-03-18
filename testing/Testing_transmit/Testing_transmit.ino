/**
 * CAN MREX Testing Transmit file 
 *
 * File:            Testing_transmit.ino
 * Organisation:    MREX
 * Author:          Chiara Gillam
 * Date Created:    10/03/2026
 * Last Modified:   14/03/2026
 * Version:         1.0.0
 *
 */

#include "CAN_MREx.h"

// -------------------------
// CAN MREx configuration
// -------------------------
uint8_t nodeID = 1;
#define TX_GPIO_NUM GPIO_NUM_14
#define RX_GPIO_NUM GPIO_NUM_13

// -------------------------
// Test state machine
// -------------------------
enum TestStage {
  TEST_INIT,
  TEST_NMT,
  TEST_HEARTBEAT,
  TEST_EMCY,
  TEST_TPDO,
  TEST_SDO,
  TEST_COMPLETE
};

TestStage currentTest = TEST_INIT;
unsigned long testTimer = 0;

// -------------------------
// Helper printing
// -------------------------
void printHeader(const char* name) {
  Serial.println();
  Serial.println("====================================");
  Serial.print("TEST: ");
  Serial.println(name);
  Serial.println("====================================");
}

void printPass(const char* msg) {
  Serial.print("[PASS] ");
  Serial.println(msg);
}

void printFail(const char* msg) {
  Serial.print("[FAIL] ");
  Serial.println(msg);
}
void printHeartbeatTable() {
  const nodeHeartbeat* table = getHeartbeatTable();

  Serial.println("----- Heartbeat Table -----");
  for (uint8_t i = 0; i < MAX_NODES; i++) {
      Serial.print("Node ");
      Serial.print(i);
      Serial.print(": Mode=0x");
      Serial.print(table[i].hbOperatingMode, HEX);
      Serial.print("  LastHB=");
      Serial.println(millis() - table[i].lastHeartbeat);
  }
  Serial.println("---------------------------");
}


// --- Helper variables ---
uint16_t speed = 0;
uint8_t brake = 0;

// -------------------------
// Setup
// -------------------------
void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("CAN MREx Test transmit Starting...");

  initCANMREX(TX_GPIO_NUM, RX_GPIO_NUM, nodeID);
  xTaskCreatePinnedToCore(
      CAN_Task,
      "CAN Task",
      6144,
      &nodeID,
      3,
      NULL,
      0
  );

  nodeOperatingMode = 0x01; // Start in Operational

  registerODEntry(0x2000, 0x01, 2, sizeof(speed), &speed); // Index, Subindex, Read Write access, Size, Data
  registerODEntry(0x2001, 0x00, 2, sizeof(brake), &brake); // Index, Subindex, Read Write access, Size, Data

  configureTPDO(0, 0x180 + nodeID, 255, 10, 10);  // COB-ID, transType, inhibit, event
  
  PdoMapEntry tpdoEntries[] = {
      {0x2000, 0x01, 16},  // Example: index 0x2000, subindex 1, 16 bits
      {0x2001, 0x00, 8}    // Example: index 0x2001, subindex 0, 8 bits
    };
  
  mapTPDO(0, tpdoEntries, 2); //TPDO 1, entries, num entries
}

// -------------------------
// Main test loop
// -------------------------
void loop() {

  switch (currentTest) {

    // ---------------------------------------------------------

    case TEST_INIT:
      printHeader("Initialisation");
      Serial.println("Checking CAN initialisation...");
      delay(500);

      sendNMT(0x01, 2);

      // Basic sanity check
      if (nodeOperatingMode == 0x01) {
        printPass("Node started in Operational mode");
      } else {
        printFail("Node did not start in STOPPED mode");
      }

      currentTest = TEST_SDO;
      delay(1000);
      break;

    // ---------------------------------------------------------

    case TEST_SDO: {
      printHeader("SDO Transmit");

      Serial.println("Writing SDO...");
      uint8_t mode = 67;
      //Write to 0x0001 0x00 the number 67
      executeSDOWrite(nodeID, 2, 0x0001, 0x00, sizeof(mode), &mode);// Node ID, targeted node, index, subindex, value, size of value
      
      Serial.println("Reading SDO...");
      uint8_t confirmation = 0;
      //Confirm it's changed
      confirmation = executeSDORead(nodeID, 2, 0x0001, 0x00);// Node ID, targeted node, index, subindex

      if (mode == confirmation) printPass("SDO read/write OK");
      else printFail("SDO read/write failed");

      currentTest = TEST_TPDO;
      delay(1000);
      break;
    }
    
    // ---------------------------------------------------------

    case TEST_TPDO: {
      printHeader("PDO Transmission");

      Serial.println("Sending TPDO1...");
      speed = 70;
      brake = 120;
      delay(10);

      uint32_t confirmSpeed32 = 0;
      uint32_t confirmBrake32 = 0;
      confirmSpeed32 = executeSDORead(nodeID, 2, 0x2000, 0x01);// Node ID, targeted node, index, subindex
      confirmBrake32 = executeSDORead(nodeID, 2, 0x2001, 0x00);// Node ID, targeted node, index, subindex
      uint16_t confirmSpeed16 = (uint16_t)confirmSpeed32;
      uint8_t confirmBrake8 =  (uint8_t)confirmBrake32;

  
      if (confirmSpeed16 == speed && confirmBrake8 == brake) printPass("PDOs OK");
      else printFail("PDOs failed");

      currentTest = TEST_EMCY;
      delay(1000);
      break;
    }

    // ---------------------------------------------------------

    case TEST_EMCY: {
      printHeader("EMCY Transmission");

      Serial.println("Sending EMCY...");
      sendEMCY(0x00, nodeID, 0x0000); // Major minor / Node ID / Type
      delay(1000);
      Serial.println("Check node 2 operating mode to see if stopped");

      
      currentTest = TEST_NMT;
      delay(1000);
      break;
    }
    // ---------------------------------------------------------

    case TEST_NMT: {
      printHeader("NMT Commands");

      Serial.println("Sending PRE-OP...");
      sendNMT(0x80, 2);
      delay(1);

      uint32_t node2_operating_mode = 0;
      node2_operating_mode = executeSDORead(nodeID, 2, 0x1000, 0x00);// Node ID, targeted node, index, subindex
      if (node2_operating_mode == 0x80) printPass("PRE-OP accepted");
      else printFail("PRE-OP failed");

      Serial.println("Sending OPERATIONAL...");
      sendNMT(0x01, 2);
      delay(1);

      node2_operating_mode = executeSDORead(nodeID, 2, 0x1000, 0x00);// Node ID, targeted node, index, subindex
      if (node2_operating_mode == 0x01) printPass("Operational accepted");
      else printFail("Operational failed");

      Serial.println("Sending STOP...");
      sendNMT(0x02, 2);
      delay(1000);
      Serial.println("Check node 2 operating mode to see if stopped");
      

      currentTest = TEST_HEARTBEAT;
      delay(1000);
      break;
    }
    // ---------------------------------------------------------


    case TEST_HEARTBEAT: {
      printHeader("Heartbeat Producer");

      printHeartbeatTable();

      // No direct API to check, so we simply confirm no crash
      printPass("Check table to confirm pass");

      currentTest = TEST_COMPLETE;
      delay(1000);
      break;
    }
    // ---------------------------------------------------------
    case TEST_COMPLETE:
      printHeader("ALL TESTS COMPLETE");
      Serial.println("CAN MREx protocol test finished.");
      while (true) delay(1000);
      break;
  }
}