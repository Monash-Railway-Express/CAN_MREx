/**
 * CAN MREX NMT_master file 
 *
 * File:            NMT_master.ino
 * Organisation:    MREX
 * Author:          Chiara Gillam
 * Date Created:    5/08/2025
 * Last Modified:   1/10/2025
 * Version:         Main Update #10
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


//OPTIONAL: timing for a non blocking function occuring every two seconds
unsigned long previousMillis = 0;
const long interval = 4000; // 2 seconds

uint8_t node2OperatingState = 0x02;

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
    uint32_t currentMs = millis();
    if (currentMs - previousMillis >= interval){
      previousMillis = currentMs;
      if (node2OperatingState == 0x02) {
        sendNMT(0x80, 0x02);
        node2OperatingState = 0x80;
      } else if (node2OperatingState == 0x80){
        sendNMT(0x01, 0x02);
        node2OperatingState = 0x01;
      } else{
        sendNMT(0x02, 0x02);
        node2OperatingState = 0x02;
      }
      Serial.println(node2OperatingState);
    }
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