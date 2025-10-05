/**
 * CAN MREX Configuration file 
 *
 * File:            CM_Config.cpp
 * Organisation:    MREX
 * Author:          Chiara Gillam
 * Date Created:    9/09/2025
 * Last Modified:   12/09/2025
 * Version:         1.10.1
 *
 */

#include "driver/twai.h"
#include "CM_ObjectDictionary.h"
#include "CM_PDO.h"


void initCANMREX(gpio_num_t TX_GPIO_NUM, gpio_num_t RX_GPIO_NUM, uint8_t nodeID){
  Serial.println("CAN MREX intialising over (TWAI)");

  // General configuration
  twai_general_config_t g_config = {
    .mode = TWAI_MODE_NORMAL,
    .tx_io = TX_GPIO_NUM,
    .rx_io = RX_GPIO_NUM,
    .clkout_io = TWAI_IO_UNUSED,
    .bus_off_io = TWAI_IO_UNUSED,
    .tx_queue_len = 5,
    .rx_queue_len = 5,
    .clkout_divider = 0,
    .intr_flags = ESP_INTR_FLAG_LEVEL1
  };

  // Timing configuration for 500 kbps
  twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();

  //Set filter for hardware filtering
  twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

  // Install and start TWAI driver
  if (twai_driver_install(&g_config, &t_config, &f_config) != ESP_OK) {
    Serial.println("TWAI driver install failed");
    while (true); // blink an led perhaps to show problem
  }
  if (twai_start() != ESP_OK) {
    Serial.println("TWAI driver start failed");
    while (true); // blink an led perhaps to show problem
  }

  //OPTIONAL: Debugging can be put in when needed
  // uint32_t alerts_to_enable = TWAI_ALERT_RX_QUEUE_FULL | TWAI_ALERT_TX_IDLE | TWAI_ALERT_BUS_ERROR;
  // if (twai_reconfigure_alerts(alerts_to_enable, NULL) == ESP_OK) {
  //   Serial.println("TWAI alerts reconfigured");
  // } else {
  //   Serial.println("Failed to reconfigure alerts");
  // }

  //Initializes all TPDOs and RPDOs as disabled and clears runtime state
  Serial.println("Initialising Default PDOs");
  initDefaultPDOs(nodeID);

  //Setup OD with default entries
  Serial.println("Initialising Default Object Dictionary");
  initDefaultOD();


 }