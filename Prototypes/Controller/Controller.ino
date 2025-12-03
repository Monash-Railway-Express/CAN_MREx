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
const uint8_t nodeID = 3;  // Change this to set your device's node ID

// --- Pin Definitions ---
#define TX_GPIO_NUM GPIO_NUM_1 // Set GPIO pin for CAN Transmit
#define RX_GPIO_NUM GPIO_NUM_2 // Set GPIO pins for CAN Receive

#define BRAKE_PIN 5
#define SPEED_PIN 6
#define BUTTON_1_PIN 8
#define BUTTON_2_PIN 3
#define SWITCH_1_PIN 17
#define SWITCH_2_PIN 18
#define DIRECTION_MODE_PIN 4
//#define CHALLENGE_MODE_PIN 36
#define CONDITION_MODE_PIN 15
#define OP_MODE_PIN 7


// --- OD definitions ---
uint16_t regenBrake=0;
uint16_t desiredSpeed=0;
uint8_t button1=0;
uint8_t button2=0;
uint8_t switch1=0;
uint8_t switch2=0;
uint8_t directionMode=0;
uint8_t conditionMode=0;
//uint8_t challengeMode=0;
uint8_t operationMode=0;


//OPTIONAL: timing for a non blocking function occuring every two seconds
unsigned long previousMillis = 0;
const long interval = 100; // 100 milliseconds

// User code end ---------------------------------------------------------


void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Serial Coms started at 115200 baud");
  pinMode(BRAKE_PIN,INPUT);
  pinMode(SPEED_PIN,INPUT);
  pinMode(BUTTON_1_PIN,INPUT);
  pinMode(BUTTON_2_PIN,INPUT);
  pinMode(SWITCH_1_PIN,INPUT);
  pinMode(SWITCH_2_PIN,INPUT);
  pinMode(DIRECTION_MODE_PIN,INPUT);
  //pinMode(CHALLENGE_MODE_PIN,INPUT);
  pinMode(CONDITION_MODE_PIN,INPUT);
  pinMode(OP_MODE_PIN,INPUT);
  analogReadResolution(10);
  
  //Initialize CANMREX protocol
  initCANMREX(TX_GPIO_NUM, RX_GPIO_NUM, nodeID);

  // User code Setup Begin: -------------------------------------------------
  // --- Register OD entries ---
  registerODEntry(0x60FF, 0x00, 2, sizeof(desiredSpeed), &desiredSpeed);
  registerODEntry(0x3012, 0x00, 2, sizeof(regenBrake), &regenBrake);
  registerODEntry(0x6060, 0x00, 2, sizeof(directionMode), &directionMode);

  // --- Register TPDOs ---
  configureTPDO(0, 0x180 + nodeID, 255, 100, 100);  // COB-ID, transType, inhibit, event
  
  PdoMapEntry tpdoEntries[] = {
      {0x60FF, 0x00, 16},  // Example: index 0x2000, subindex 1, 16 bits
      {0x3012, 0x00, 16}    // Example: index 0x2001, subindex 0, 8 bits
    };
  mapTPDO(0, tpdoEntries, 2); //TPDO 1, entries, num entries

  // --- Register RPDOs ---
 

  // User code Setup end ------------------------------------------------------
}


void loop() {
  //print_status();
  //Serial.println("Preop Mode");
  //User Code begin loop() ----------------------------------------------------
  // --- Stopped mode (This is default starting point) ---
  if (nodeOperatingMode == 0x02){ 
    handleCAN(nodeID);
    Serial.println("Stopped Mode");
    int opMode = check5Switch(analogRead(OP_MODE_PIN));
    if(opMode == 2){
      sendNMT(0x80, 0x01);
      nodeOperatingMode = 0x80;
    }
  }

  // --- Pre operational state (This is where you can do checks and make sure that everything is okay) ---
  if (nodeOperatingMode == 0x80){ 
    handleCAN(nodeID);
    Serial.println("Preop Mode");
    int opMode = check5Switch(analogRead(OP_MODE_PIN));

    if(opMode == 1){
      sendNMT(0x02, 0x01);
      nodeOperatingMode = 0x02;

    }
      
    if(opMode == 3){
      sendNMT(0x01, 0x01);
      nodeOperatingMode = 0x01;
    }
  }

  // --- Operational state (Normal operating mode) ---
  if (nodeOperatingMode == 0x01){ 
    handleCAN(nodeID);
    Serial.println("Normal Mode");
    int opMode = check5Switch(analogRead(OP_MODE_PIN));
    if(opMode == 2){
      sendNMT(0x80, 0x02);
      nodeOperatingMode = 0x80;
    }


    //directionMode = check3Switch(analogRead(DIRECTION_MODE_PIN));
    //Serial.println(directionMode);

    regenBrake = analogRead(BRAKE_PIN);
    Serial.println(regenBrake);
    desiredSpeed = analogRead(SPEED_PIN);
    Serial.println(desiredSpeed);
    // unsigned long currentMillis = millis();
    // if (currentMillis - previousMillis >= interval) {
    //   previousMillis = currentMillis;
    //   Serial.print("Speed: ");
    //   Serial.print(desiredSpeed);
    //   Serial.print("\tBrake: ");
    //   Serial.println(regenBrake);
    // }

  }

  //User code end loop() --------------------------------------------------------
}

void print_status(){
  //Check readings of brake and speed
  regenBrake = analogRead(BRAKE_PIN);
  desiredSpeed = analogRead(SPEED_PIN);
  Serial.print("Speed: ");
  Serial.print(desiredSpeed);
  Serial.print("\tBrake: ");
  Serial.println(regenBrake);

  //Check buttons
  button1 = digitalRead(BUTTON_1_PIN);
  button2 = digitalRead(BUTTON_2_PIN);
  Serial.print("Button 1: ");
  Serial.print(button1);
  Serial.print("\tButton 2: ");
  Serial.println(button2);

  //Check switches
  switch1 = digitalRead(SWITCH_1_PIN);
  switch2 = digitalRead(SWITCH_2_PIN);
  Serial.print("Switch 1: ");
  Serial.print(switch1);
  Serial.print("\tSwitch 2: ");
  Serial.println(switch2);

  //Check position switches 
  directionMode = analogRead(DIRECTION_MODE_PIN);
  conditionMode = analogRead(CONDITION_MODE_PIN);
  //challengeMode = analogRead(CHALLENGE_MODE_PIN);
  operationMode = analogRead(OP_MODE_PIN);

  Serial.print("Direction: ");
  Serial.print(directionMode);
  Serial.print("\tCondition: ");
  Serial.println(conditionMode);
  //Serial.print("Challenge: ");
  //Serial.print(challengeMode);
  Serial.print("\t\tOperation: ");
  Serial.println(operationMode);

}

int check3Switch(int read){
  //Serial.println(read);
  if(read<100){
    //Reverse
    return 2;
  }
  else if(read<250){
    //Forward
    return 1;
  }
  else{
    //Neutral
    return 0;
  }
}
 
 int check5Switch(int read) {
  //Serial.println(read);
  if(read<150){
    //stopped
    return 1;
  }
  else if(read<300){
    //pre op 
    return 2;
  }
  else if(read<400){
    
    return 3;
  }
  else if(read<600){
    //op
    return 4;
  }
  else{
    return 5;
  }
 }

