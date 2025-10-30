/**
 * CAN MREX main (Template) file 
 *
 * File:            main.ino
 * Organisation:    MREX
 * Author:          Chiara Gillam
 * Date Created:    5/08/2025
 * Last Modified:   1/10/2025
 * Version:         1.11.0
 *
 */


#include "CM.h" // inlcudes all CAN MREX files
#include "driver/pcnt.h"
#include <esp_system.h>
#include <Ticker.h>


// User code begin: ------------------------------------------------------
// --- CAN MREx initialisation ---
const uint8_t nodeID = 1;  // Change this to set your device's node ID

// --- Pin Definitions ---
#define TX_GPIO_NUM GPIO_NUM_5 // Set GPIO pin for CAN Transmit
#define RX_GPIO_NUM GPIO_NUM_4 // Set GPIO pins for CAN Receive

// Hardware configuration
#define ENCODER_PIN 17
#define PCNT_UNIT PCNT_UNIT_0
#define PCNT_CHANNEL PCNT_CHANNEL_0
#define PCNT_HIGH_LIMIT 1000000
#define PCNT_LOW_LIMIT 0


unsigned long lastRPMUpdateMicros = 0;
unsigned long lastPrintMicros = 0;
int16_t lastPulseCount = 0;
uint16_t currentRPM = 0;


const unsigned int pulsesPerRev = 200; // Encoder pulses per revolution

// --- OD definitions ---


// User code end ---------------------------------------------------------


// ---------------------------
// PCNT (pulse counter) setup
// ---------------------------
void setupPCNT() {
  pcnt_config_t pcnt_config = {};
  pcnt_config.pulse_gpio_num = ENCODER_PIN;
  pcnt_config.ctrl_gpio_num = PCNT_PIN_NOT_USED;
  pcnt_config.channel = PCNT_CHANNEL;
  pcnt_config.unit = PCNT_UNIT;
  pcnt_config.pos_mode = PCNT_COUNT_INC;   // count on rising edge
  pcnt_config.neg_mode = PCNT_COUNT_DIS;   // ignore falling edge
  pcnt_config.lctrl_mode = PCNT_MODE_KEEP;
  pcnt_config.hctrl_mode = PCNT_MODE_KEEP;
  pcnt_config.counter_h_lim = PCNT_HIGH_LIMIT;
  pcnt_config.counter_l_lim = PCNT_LOW_LIMIT;

  esp_err_t err = pcnt_unit_config(&pcnt_config);
  if (err != ESP_OK) {
    Serial.printf("pcnt_unit_config failed: %d\n", err);
  }

  // Disable PCNT filter so we don't accidentally drop legitimate pulses.
  // (If your SDK doesn't provide pcnt_filter_disable, you can set a very small filter value.)
  pcnt_filter_disable(PCNT_UNIT);

  pcnt_counter_pause(PCNT_UNIT);
  pcnt_counter_clear(PCNT_UNIT);
  pcnt_counter_resume(PCNT_UNIT);
}



void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Serial Coms started at 115200 baud");
  
  //Initialize CANMREX protocol
  initCANMREX(TX_GPIO_NUM, RX_GPIO_NUM, nodeID);

  // Setup PCNT
  setupPCNT();

  // User code Setup Begin: -------------------------------------------------
  // --- Register OD entries ---
  registerODEntry(0x606C, 0x00, 2, sizeof(currentRPM), &currentRPM);

  // --- Register TPDOs ---
  configureTPDO(0, 0x180 + nodeID, 255, 10, 100);  // COB-ID, transType, inhibit, event
  
  PdoMapEntry tpdoEntries[] = {
      {0x606C, 0x00, 16},  // index 0x606C, subindex 0, 16 bits
    };
  
  mapTPDO(0, tpdoEntries, 1); //TPDO 1, entries, num entries

  // --- Register RPDOs ---


  // --- Set pin modes ---
 

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

    unsigned long nowMicros = micros();

    // Update RPM every 50 ms
    if (nowMicros - lastRPMUpdateMicros >= 200000) {
      int16_t pulseCount = 0;
      pcnt_get_counter_value(PCNT_UNIT, &pulseCount);

      unsigned long elapsedMicros = nowMicros - lastRPMUpdateMicros;
      int16_t deltaPulses = pulseCount - lastPulseCount;

      float rpmFloat = (deltaPulses * 60.0 * 1e6) / (pulsesPerRev * elapsedMicros);
      currentRPM = (uint16_t)rpmFloat;  // Cast to uint16_t

      lastPulseCount = pulseCount;
      lastRPMUpdateMicros = nowMicros;
    }

    // Print RPM every 1 second
    if (nowMicros - lastPrintMicros >= 100000) {
      Serial.printf("RPM: %u\n", currentRPM);
      lastPrintMicros = nowMicros;
    }
  }

  //User code end loop() --------------------------------------------------------
}