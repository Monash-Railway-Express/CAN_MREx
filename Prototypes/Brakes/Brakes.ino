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
const uint8_t nodeID = 2;  // Change this to set your device's node ID

// --- Pin Definitions ---
#define TX_GPIO_NUM GPIO_NUM_5 // Set GPIO pin for CAN Transmit
#define RX_GPIO_NUM GPIO_NUM_4 // Set GPIO pins for CAN Receive
#define SERVICE_BRAKE_PIN GPIO_NUM_17 

// --- OD definitions ---
uint8_t serviceBrake = 0;



// User code end ---------------------------------------------------------


void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Serial Coms started at 115200 baud");
  
  //Initialize CANMREX protocol
  initCANMREX(TX_GPIO_NUM, RX_GPIO_NUM, nodeID);

  // User code Setup Begin: -------------------------------------------------
  // --- Register OD entries ---
  registerODEntry(0x3012, 0x01, 2, sizeof(serviceBrake), &serviceBrake);



  // --- Register TPDOs ---
  

  // --- Register RPDOs ---
  configureRPDO(0, 0x180 + 1, 255, 0);         // COB-ID, transType, inhibit

  PdoMapEntry rpdoEntries[] = {
    {0x3012, 0x01, 8}  // Example: index 0x2000, subindex 1, 16 bits
  };
  mapRPDO(0, rpdoEntries, 1);
  
  // --- Set pin modes ---
  pinMode(SERVICE_BRAKE_PIN, OUTPUT);

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
    if(serviceBrake != 0){
      digitalWrite(SERVICE_BRAKE_PIN, HIGH);
      Serial.println("Service brake on");
    }
    else{
      digitalWrite(SERVICE_BRAKE_PIN, LOW);
      Serial.println("Service brake off");
    }
  }

  //User code end loop() --------------------------------------------------------
}