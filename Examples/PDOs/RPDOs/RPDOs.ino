/**
 * CAN MREX RPDOs file 
 *
 * File:            RPDOs.ino
 * Organisation:    MREX
 * Author:          Chiara Gillam
 * Date Created:    1/10/2025
 * Last Modified:   15/10/2025
 * Version:         1.11.0
 *
 */


#include "CM.h" // inlcudes all CAN MREX files

// User code begin: ------------------------------------------------------
// --- CAN MREx initialisation ---
const uint8_t nodeID = 2;  // Change this to set your device's node ID

// --- Pin Definitions ---
#define TX_GPIO_NUM GPIO_NUM_5 // Set GPIO pin for CAN Transmit
#define RX_GPIO_NUM GPIO_NUM_4 // Set GPIO pins for CAN Receive

// --- OD definitions ---
uint16_t speed = 0;
uint8_t brake = 0;

//OPTIONAL: timing for a non blocking function occuring every two seconds
unsigned long previousMillis = 0;
const long interval = 2000; // 2 seconds

// User code end ---------------------------------------------------------


void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Serial Coms started at 115200 baud");
  
  //Initialize CANMREX protocol
  initCANMREX(TX_GPIO_NUM, RX_GPIO_NUM, nodeID);

  // User code Setup Begin: -------------------------------------------------
  // --- Register OD entries ---
  registerODEntry(0x2000, 0x01, 2, sizeof(uint16_t), &speed);
  registerODEntry(0x2001, 0x00, 2, sizeof(uint8_t), &brake); 

  // --- Register TPDOs ---
  

  // --- Register RPDOs ---
  configureRPDO(0, 0x180 + 1, 255, 0);         // COB-ID, transType, inhibit

  PdoMapEntry rpdoEntries[] = {
    {0x2000, 0x01, 16},  // Example: index 0x2000, subindex 1, 16 bits
    {0x2001, 0x00, 8}    // Example: index 0x2001, subindex 0, 8 bits
  };
  mapRPDO(0, rpdoEntries, 2);

  nodeOperatingMode = 0x01;

  // User code Setup end ------------------------------------------------------
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
    }
  }

  //User code end loop() --------------------------------------------------------
}