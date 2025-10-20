/**
 * CAN MREX SDOs file 
 *
 * File:            CM_SDO.h
 * Organisation:    MREX
 * Author:          Chiara Gillam
 * Date Created:    30/09/2025
 * Last Modified:   15/10/2025
 * Version:         1.11.0
 *
 */


#ifndef CM_SDO_H
#define CM_SDO_H


void handleSDO(const twai_message_t& rxMsg, uint8_t nodeID);
void transmitSDO(uint8_t nodeID, uint8_t targetNodeID, uint8_t* data, uint32_t* outValue);
void prepareSDOTransmit(uint8_t cmd, uint16_t index, uint8_t subindex, const void* value, size_t size, uint8_t* outBuf);
void executeSDOWrite(uint8_t nodeID, uint8_t targetNodeID, uint16_t index, uint8_t subindex,  const void* value, size_t size);
uint32_t executeSDORead(uint8_t nodeID, uint8_t targetNodeID, uint16_t index, uint8_t subindex);
void waitSDOResponse(uint32_t* outValue, uint8_t targetNodeID, uint8_t nodeID);

#endif