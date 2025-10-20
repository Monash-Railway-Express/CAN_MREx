/**
 * CAN MREX Configuration file 
 *
 * File:            CM_Config.h
 * Organisation:    MREX
 * Author:          Chiara Gillam
 * Date Created:    9/09/2025
 * Last Modified:   9/09/2025
 * Version:         1.11.0
 *
 */

#ifndef CM_CONFIG_H
#define CM_CONFIG_H

#include <Arduino.h>
#include "driver/twai.h"


void initCANMREX(gpio_num_t TX_GPIO_NUM, gpio_num_t RX_GPIO_NUM, uint8_t nodeID);


#endif
