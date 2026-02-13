/**
 * CAN MREx EMCY_alert file 
 *
 * File:            EMCY_alert.ino
 * Organisation:    MREx
 * Author:          Chiara Gillam
 * Date Created:    5/08/2025
 * Last Modified:   14/01/2026
 * Version:         1.12.1
 *
 */

#include <CAN_MREx.h> // inlcudes all CAN MREx files

// User code begin: ------------------------------------------------------
// --- CAN MREx initialisation
const uint8_t nodeID = 1;  // Change this to set your device's node ID

// --- Pin Definitions ---
#define TX_GPIO_NUM GPIO_NUM_5 // Set GPIO pin for CAN Transmit
#define RX_GPIO_NUM GPIO_NUM_4 // Set GPIO pins for CAN Receive

// --- OD definitions ---


// User code end ---------------------------------------------------------


void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Serial Coms started at 115200 baud");
  
  //Initialize CANMREX protocol
  initCANMREX(TX_GPIO_NUM, RX_GPIO_NUM, nodeID);

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
    handleCAN(nodeID);
    Serial.println("Emergency stop in 5 seconds");
    delay(5000);
    Serial.println("Emergency stop");
    sendEMCY(0x00, nodeID, 0x0000); // Major minor / Node ID / Type
    delay(20000);
  }

  // --- Pre operational state (This is where you can do checks and make sure that everything is okay) ---
  if (nodeOperatingMode == 0x80){ 
    handleCAN(nodeID);
  }

  // --- Operational state (Normal operating mode) ---
  if (nodeOperatingMode == 0x01){ 
    handleCAN(nodeID);
  }

  //User code end loop() --------------------------------------------------------
}