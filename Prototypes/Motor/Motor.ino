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
#include <Arduino.h>

// User code begin: ------------------------------------------------------
// --- CAN MREx initialisation ---
const uint8_t nodeID = 1;  // Change this to set your device's node ID

// --- Pin Definitions ---
#define TX_GPIO_NUM GPIO_NUM_5 // Set GPIO pin for CAN Transmit
#define RX_GPIO_NUM GPIO_NUM_4 // Set GPIO pins for CAN Receive
#define REGEN_BRAKE_PIN GPIO_NUM_25
#define MOTOR_PIN GPIO_NUM_26

// --- OD definitions ---
uint16_t desiredSpeed = 0;
uint16_t regenBrake = 0;
uint8_t serviceBrake = 0;

//OPTIONAL: timing for a non blocking function occuring every two seconds
unsigned long previousMillis = 0;
const long interval = 100; // 100 milliseconds

// setting PWM properties
const int freq = 5000;
const int resolution = 8;
 


// User code end ---------------------------------------------------------


void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Serial Coms started at 115200 baud");
  
  //Initialize CANMREX protocol
  initCANMREX(TX_GPIO_NUM, RX_GPIO_NUM, nodeID);

  // User code Setup Begin: -------------------------------------------------
  // --- Register OD entries ---
  registerODEntry(0x60FF, 0x00, 2, sizeof(desiredSpeed), &desiredSpeed);
  registerODEntry(0x3012, 0x00, 2, sizeof(regenBrake), &regenBrake);
  registerODEntry(0x3012, 0x01, 2, sizeof(serviceBrake), &serviceBrake);


  // --- Register TPDOs ---
  configureTPDO(0, 0x180 + nodeID, 255, 100, 100);  // COB-ID, transType, inhibit, event
  
  PdoMapEntry tpdoEntries[] = {
      {0x3012, 0x01, 8},  // Example: index 0x2000, subindex 1, 16 bits
    };
  mapTPDO(0, tpdoEntries, 1); //TPDO 1, entries, num entries

  // --- Register RPDOs ---
  configureRPDO(0, 0x180 + 3, 255, 0);         // COB-ID, transType, inhibit

  PdoMapEntry rpdoEntries[] = {
    {0x60FF, 0x00, 16},  // Example: index 0x2000, subindex 1, 16 bits
    {0x3012, 0x00, 16}    // Example: index 0x2001, subindex 0, 8 bits
  };
  mapRPDO(0, rpdoEntries, 2);

  nodeOperatingMode = 0x01;

  // --- Set pin modes ---
  ledcAttach(MOTOR_PIN, freq, resolution);
  ledcAttach(REGEN_BRAKE_PIN, freq, resolution);


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

    // Declare variables outside the conditional blocks
    uint8_t motorpwmValue = 0;
    uint8_t brakepwmValue = 0;

    // Decision logic
    if(desiredSpeed > 10 && regenBrake <= 10){
      motorpwmValue = desiredSpeed >> 4;
      brakepwmValue = 0;
      serviceBrake = 0;
    }
    else if(desiredSpeed > 10 && regenBrake > 10){
      motorpwmValue = 0;
      brakepwmValue = regenBrake >> 4;
      serviceBrake = 0;
    }
    else if(desiredSpeed <= 10 && regenBrake > 10){
      motorpwmValue = 0;
      brakepwmValue = regenBrake >> 4;
      serviceBrake = 1;
    }
    else{
      motorpwmValue = 0;
      brakepwmValue = 0;
      serviceBrake = 1;
    }

    // Apply PWM outputs
    ledcWrite(MOTOR_PIN, motorpwmValue);
    ledcWrite(REGEN_BRAKE_PIN, brakepwmValue);

    // Debug output
    Serial.print("Motor value: ");
    Serial.println(motorpwmValue);
    Serial.print("Brake value: ");
    Serial.println(brakepwmValue);
    Serial.print("Service brake: ");
    Serial.println(serviceBrake);
  }
}

  //User code end loop() --------------------------------------------------------
}