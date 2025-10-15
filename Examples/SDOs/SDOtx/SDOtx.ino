/**
 * CAN MREX main (Template) file 
 *
 * File:            main.ino
 * Organisation:    MREX
 * Author:          Chiara Gillam
 * Date Created:    5/08/2025
 * Last Modified:   1/10/2025
 * Version:         1.10.2
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
uint8_t mode = 0;
uint32_t heartbeatNode2;

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


  // --- Register TPDOs ---
  

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

      // Write mode to node 2 (Speed in the dictionary)
      executeSDOWrite(nodeID, 2, 0x0001, 0x00, &mode, sizeof(mode));// Node ID, targeted node, index, subindex, value, size of value

      Serial.print("Mode transmitted: ");
      Serial.print(mode);
      Serial.println();
      mode++;
      

      // Read heartbeat interval from node 2 (index 0x1017, subindex 0x00)
      executeSDORead(nodeID, 2, 0x1017, 0x00, &heartbeatNode2);// Node ID, targeted node, index, subindex, variable to return to 

      Serial.print("Heartbeat from node 2 Received: ");
      Serial.print(heartbeatNode2);
      Serial.println();
    }
  }

  //User code end loop() --------------------------------------------------------
}