/**
 * CAN MREX Network managment tool file
 *
 * File:            CM_NMT.h
 * Organisation:    MREX
 * Author:          Chiara Gillam
 * Date Created:    12/09/2025
 * Last Modified:   12/09/2025
 * Version:         1.10.1
 *
 */

#ifndef CM_NMT_H
#define CM_NMT_H

void handleNMT(const twai_message_t& rxMsg, uint8_t nodeID);
void sendNMT(uint8_t sendOperatingMode, uint8_t targetNodeID);

#endif