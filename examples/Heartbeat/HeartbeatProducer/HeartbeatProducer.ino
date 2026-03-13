/**
 * CAN MREX HeartbeatProducer file 
 *
 * File:            HeartbeatProducer.ino
 * Organisation:    MREx
 * Author:          Chiara Gillam
 * Date Created:    5/08/2025
 * Last Modified:   14/03/2026
 * Version:         1.13.0
 *
 */

#include <CAN_MREx.h> // Inlcudes all CAN MREX files

// User code begin: ------------------------------------------------------

// --- CAN MREx variables ---
uint8_t nodeID = 1;  // Change this to set your device's node ID
#define TX_GPIO_NUM GPIO_NUM_5 // Set GPIO pin for CAN Transmit
#define RX_GPIO_NUM GPIO_NUM_4 // Set GPIO pins for CAN Receive

// --- Pin Definitions ---

// --- OD definitions ---


//OPTIONAL: timing for a non blocking function occuring every two seconds
unsigned long previousMillis = 0;
const long interval = 10000; // 10 seconds

// User code end ---------------------------------------------------------

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Serial Coms started at 115200 baud");
  
  //Initialize CANMREX protocol
  initCANMREX(TX_GPIO_NUM, RX_GPIO_NUM, nodeID);
  xTaskCreatePinnedToCore(
      CAN_Task,
      "CAN Task",
      4096,
      &nodeID,   // <--- passed into pvParameters
      3,
      NULL,
      0
  );

  // User code Setup Begin: -------------------------------------------------
  // --- Register OD entries ---


  // --- Register TPDOs ---
  

  // --- Register RPDOs ---
 

  // User code Setup end ------------------------------------------------------
  
}

void loop() {
  //User Code begin loop() ----------------------------------------------------
  // --- Stopped mode (This is default starting point) ---
  if (nodeOperatingMode == 0x02){ 
    uint32_t currentMs = millis();
    if (currentMs - previousMillis >= interval){ //cause a block longer than 1.5s to simulate node dropping out 
      previousMillis = currentMs;
      Serial.println("Simulating node dropout");
      delay(2000);
    }
  }

  // --- Pre operational state (This is where you can do checks and make sure that everything is okay) ---
  if (nodeOperatingMode == 0x80){ 
  }

  // --- Operational state (Normal operating mode) ---
  if (nodeOperatingMode == 0x01){
  }

  //User code end loop() --------------------------------------------------------
}