/**
 * CAN MREX Emergency file
 *
 * File:            CM_EMCY.h
 * Organisation:    MREX
 * Author:          Chiara Gillam
 * Date Created:    12/09/2025
 * Last Modified:   13/09/2025
 * Version:         1.13.0
 *
 */

#ifndef CM_EMCY_H
#define CM_EMCY_H

#include <Arduino.h>
#include <driver/twai.h>

bool CheckMinorEMCY();
bool CheckMajorEMCY();

bool GetMinorByIndex(uint8_t index, uint8_t* node, uint32_t* code);
bool GetMajorByIndex(uint8_t index, uint8_t* node, uint32_t* code);

void HandleEMCY(const twai_message_t& rxMsg, uint8_t nodeID);
void sendEMCY(uint8_t priority, uint8_t nodeID, uint32_t errorCode);

#endif