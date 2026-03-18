/**
 * CAN MREX HeartbeatConsumer file 
 *
 * File:            HeartbeatConsumer.ino
 * Organisation:    MREX
 * Author:          Chiara Gillam
 * Date Created:    5/08/2025
 * Last Modified:   14/03/2026
 * Version:         1.13.0
 *
 */

// Heart beat consumer is not currently available

#include <CAN_MREx.h> // inlcudes all CAN MREX files

// User code begin: ------------------------------------------------------

uint8_t nodeID = 2;  // Change this to set your device's node ID

// --- Pin Definitions ---
#define TX_GPIO_NUM GPIO_NUM_5 // Set GPIO pin for CAN Transmit
#define RX_GPIO_NUM GPIO_NUM_4 // Set GPIO pins for CAN Receive

// --- OD definitions ---


//OPTIONAL: timing for a non blocking function occuring every two seconds
// unsigned long previousMillis = 0;
// const long interval = 2000; // 2 seconds

// User code end ---------------------------------------------------------

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Serial Coms started at 115200 baud");
  
  //Initialize CANMREX protocol
  initCANMREX(TX_GPIO_NUM, RX_GPIO_NUM, nodeID);
  enableHeartbeatMonitoring(true); // This enables heartbeat consumption
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


  // --- Register TPDOs ---
  

  // --- Register RPDOs ---
 

  // User code Setup end ------------------------------------------------------

  nodeOperatingMode = 0x01;
}

void loop() {
  //User Code begin loop() ----------------------------------------------------
  // --- Stopped mode (This is default starting point) ---
  if (nodeOperatingMode == 0x02){ 
    Serial.println("Emegency stop");
    delay(1000);
  }

  // --- Pre operational state (This is where you can do checks and make sure that everything is okay) ---
  if (nodeOperatingMode == 0x80){ 
  }

  // --- Operational state (Normal operating mode) ---
  if (nodeOperatingMode == 0x01){ 
  }

  //User code end loop() --------------------------------------------------------
}