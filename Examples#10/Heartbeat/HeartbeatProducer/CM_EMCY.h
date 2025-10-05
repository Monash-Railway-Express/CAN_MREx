/**
 * CAN MREX Emergency file
 *
 * File:            CM_EMCY.h
 * Organisation:    MREX
 * Author:          Chiara Gillam
 * Date Created:    12/09/2025
 * Last Modified:   13/09/2025
 * Version:         1.10.1
 *
 */

#ifndef CM_EMCY_H
#define CM_EMCY_H

void handleEMCY(const twai_message_t& rxMsg, uint8_t nodeID);
void sendEMCY(uint8_t priority, uint8_t nodeID, uint32_t errorCode);

#endif