/**
 * CAN MREX NMT_slave file 
 *
 * File:            NMT_slave.ino
 * Organisation:    MREX
 * Author:          Chiara Gillam
 * Date Created:    5/08/2025
 * Last Modified:   1/10/2025
 * Version:         Main Update #10
 *
 */

#include "CM.h" // inlcudes all CAN MREX files

// User code begin: ------------------------------------------------------

const uint8_t nodeID = 2;  // Change this to set your device's node ID
const bool nmtMaster = false;
const bool heartbeatConsumer = false;

// --- Pin Definitions ---
#define TX_GPIO_NUM GPIO_NUM_5 // Set GPIO pin for CAN Transmit
#define RX_GPIO_NUM GPIO_NUM_4 // Set GPIO pins for CAN Receive

const int led = 17;

// --- OD definitions ---


//OPTIONAL: timing for a non blocking function occuring every two seconds
unsigned long previousMillis = 0;
const long intervalPreOp = 700; 
const long intervalOp = 250;
bool ledState = false;

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


  pinMode(led, OUTPUT);

  // User code Setup end ------------------------------------------------------


}

void loop() {
  //User Code begin loop() ----------------------------------------------------
  // --- Stopped mode (This is default starting point) ---
  if (nodeOperatingMode == 0x02){ 
    handleCAN(nodeID);

  }

  // --- Pre operational state (This is where you can do checks and make sure that everything is okay) ---
  if (nodeOperatingMode == 0x80) {
    handleCAN(nodeID);
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= intervalPreOp) {
      previousMillis = currentMillis;

      // Toggle the LED state
      ledState = !ledState;
      digitalWrite(led, ledState ? HIGH : LOW);
    }
  }

  // --- Operational state (Normal operating mode) ---
  if (nodeOperatingMode == 0x01){ 
    handleCAN(nodeID);
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= intervalOp) {
      previousMillis = currentMillis;

      // Toggle the LED state
      ledState = !ledState;
      digitalWrite(led, ledState ? HIGH : LOW);
    }
  }

  //User code end loop() --------------------------------------------------------
}