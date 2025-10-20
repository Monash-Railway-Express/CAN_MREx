/**
 * CAN MREX TPDOs file 
 *
 * File:            TPDOs.ino
 * Organisation:    MREX
 * Author:          Chiara Gillam
 * Date Created:    1/10/2025
 * Last Modified:   20/10/2025
 * Version:         1.11.0
 *
 */


#include "CM.h" // inlcudes all CAN MREX files

// User code begin: ------------------------------------------------------
// --- CAN MREx initialisation ---
const uint8_t nodeID = 1;  // Change this to set your device's node ID

// --- Pin Definitions ---
#define TX_GPIO_NUM GPIO_NUM_5 // Set GPIO pin for CAN Transmit
#define RX_GPIO_NUM GPIO_NUM_4 // Set GPIO pins for CAN Receive

// --- OD definitions ---
uint16_t speed = 0;
uint8_t brake = 0;

//OPTIONAL: timing for a non blocking function occuring every two seconds
unsigned long previousMillis = 0;
const long interval = 1000; // 1 second

// User code end ---------------------------------------------------------


void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Serial Coms started at 115200 baud");
  
  //Initialize CANMREX protocol
  initCANMREX(TX_GPIO_NUM, RX_GPIO_NUM, nodeID);

  // User code Setup Begin: -------------------------------------------------
  // --- Register OD entries ---
  registerODEntry(0x2000, 0x01, 2, sizeof(speed), &speed);
  registerODEntry(0x2001, 0x00, 2, sizeof(brake), &brake);


  // --- Register TPDOs ---
  configureTPDO(0, 0x180 + nodeID, 255, 10, 10);  // COB-ID, transType, inhibit, event
  
  PdoMapEntry tpdoEntries[] = {
      {0x2000, 0x01, 16},  // Example: index 0x2000, subindex 1, 16 bits
      {0x2001, 0x00, 8}    // Example: index 0x2001, subindex 0, 8 bits
    };
  
  mapTPDO(0, tpdoEntries, 2); //TPDO 1, entries, num entries

  // --- Register RPDOs ---
 

  // User code Setup end ------------------------------------------------------

  nodeOperatingMode = 0x01;
}


void loop() {
  //User Code begin loop() ----------------------------------------------------
  // --- Stopped mode (This is default starting point) ---
  if (nodeOperatingMode == 0x02){ 
    handleCAN(nodeID);
  }

  // --- Pre operational state (This is where you can do checks and make sure that everything is okay) ---
  if (nodeOperatingMode == 0x80){ 
    handleCAN(nodeID);
  }

  // --- Operational state (Normal operating mode) ---
  if (nodeOperatingMode == 0x01){ 
    handleCAN(nodeID);
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      Serial.print("Speed: ");
      Serial.println(speed);
      Serial.print("Brake: ");
      Serial.println(brake);
      speed += 2;
      brake += 1;
    }
  }

  //User code end loop() --------------------------------------------------------
}