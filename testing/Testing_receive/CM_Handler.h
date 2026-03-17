/**
 * CAN MREX Handler file 
 *
 * File:            CM_Handler.h
 * Organisation:    MREX
 * Author:          Chiara Gillam
 * Date Created:    6/08/2025
 * Last Modified:   10/03/2026
 * Version:         1.13.0
 *
 */


#ifndef CM_HANDLER_H
#define CM_HANDLER_H

#include <Arduino.h>
#include <driver/twai.h>

void handleCAN(uint8_t nodeID, twai_message_t* pdoMsg = nullptr);


#endif