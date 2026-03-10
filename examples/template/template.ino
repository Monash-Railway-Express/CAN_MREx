/**
 * CAN MREX Template file 
 *
 * File:            template.ino
 * Organisation:    MREX
 * Author:          Chiara Gillam
 * Date Created:    10/03/2026
 * Last Modified:   10/03/2026
 * Version:         1.12.3
 *
 */

#include <CAN_MREx.h> // Inlcudes all CAN MREX files

// User code begin: ------------------------------------------------------
//Includes



// --- CAN MREx initialisation ---
const uint8_t nodeID = 3;  // Change this to set your device's node ID

// --- Pin Definitions ---
#define TX_GPIO_NUM GPIO_NUM_4 // Set GPIO pin for CAN Transmit
#define RX_GPIO_NUM GPIO_NUM_5 // Set GPIO pins for CAN Receive


// --- OD definitions ---


//OPTIONAL: timing for a non blocking function occuring every two seconds
unsigned long previousMillis = 0;
const long interval = 100; // 100 milliseconds

// User code end ---------------------------------------------------------


void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Serial Coms started at 115200 baud");

  //Initialize CANMREX protocol
  initCANMREX(TX_GPIO_NUM, RX_GPIO_NUM, nodeID);
  nodeOperatingMode = 0x02; // Default operating mode 

  // User code Setup Begin: -------------------------------------------------
  // --- Register OD entries ---


  // --- Register TPDOs ---


  // --- Register RPDOs ---

  
  // User code Setup end ---------------------------------------------------------
}


void loop() {
  // //User Code begin loop() ----------------------------------------------------
  handleCAN(nodeID);
  
  // --- Stopped mode (This is default starting point) ---
  if (nodeOperatingMode == 0x02){ 
    Serial.println("Stopped Mode");
    
  }

  // --- Pre operational state (This is where you can do checks and make sure that everything is okay) ---
  if (nodeOperatingMode == 0x80){ 
    Serial.println("Preop Mode");
  }

  // --- Operational state (Normal operating mode) ---
  if (nodeOperatingMode == 0x01){ 
    Serial.println("Normal Mode");

  }

  //User code end loop() --------------------------------------------------------
}