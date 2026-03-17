/**
 * CAN MREX Testing receive file 
 *
 * File:            Testing_receive.ino
 * Organisation:    MREX
 * Author:          Chiara Gillam
 * Date Created:    10/03/2026
 * Last Modified:   14/03/2026
 * Version:         1.13.0
 *
 */

#include "CAN_MREx.h" // Inlcudes all CAN MREX files

// User code begin: ------------------------------------------------------
//Includes



// --- CAN MREx variables ---
uint8_t nodeID = 2;  // Change this to set your device's node ID
#define TX_GPIO_NUM GPIO_NUM_5 // Set GPIO pin for CAN Transmit
#define RX_GPIO_NUM GPIO_NUM_4 // Set GPIO pins for CAN Receive


// --- Pin Definitions ---



// --- OD definitions ---
uint16_t speed = 0;
uint8_t brake = 0;
uint8_t start_timeout = 0;
uint8_t mode = 0;





// User code end ---------------------------------------------------------


void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Serial Coms started at 115200 baud");

  //Initialize CANMREX protocol
  initCANMREX(TX_GPIO_NUM, RX_GPIO_NUM, nodeID);
  nodeOperatingMode = 0x01; // Start in Operational
  xTaskCreatePinnedToCore(
      CAN_Task,
      "CAN Task",
      4096,
      &nodeID,
      3,
      NULL,
      0
  );
  
  

  // User code Setup Begin: -------------------------------------------------
  // --- Register OD entries ---
  registerODEntry(0x2000, 0x01, 2, sizeof(speed), &speed);
  registerODEntry(0x2001, 0x00, 2, sizeof(brake), &brake); 
  registerODEntry(0x0001, 0x00, 2, sizeof(mode), &mode); 

  // --- Register TPDOs ---


  // --- Register RPDOs ---
  configureRPDO(0, 0x180 + 1, 255, 0);         // COB-ID, transType, inhibit

  PdoMapEntry rpdoEntries[] = {
    {0x2000, 0x01, 16},  // Example: index 0x2000, subindex 1, 16 bits
    {0x2001, 0x00, 8}    // Example: index 0x2001, subindex 0, 8 bits
  };
  mapRPDO(0, rpdoEntries, 2);
  
  // User code Setup end ---------------------------------------------------------
}


void loop() {
  // //User Code begin loop() ----------------------------------------------------
  Serial.println(nodeOperatingMode);
  delay(200);

  //User code end loop() --------------------------------------------------------
}