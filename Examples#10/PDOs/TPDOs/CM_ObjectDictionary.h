/**
 * CAN MREX Object Dictionary file 
 *
 * File:            CM_ObjectDictionary.h
 * Organisation:    MREX
 * Author:          Chiara Gillam
 * Date Created:    6/08/2025
 * Last Modified:   12/09/2025
 * Version:         1.10.1
 *
 */

 

#ifndef CM_OBJECT_DICTIONARY_H
#define CM_OBJECT_DICTIONARY_H

#include <stdint.h>

extern uint8_t nodeOperatingMode;  // set operating mode to 0x02 initially
extern uint32_t heartbeatInterval;

typedef struct {
  uint16_t index;
  uint8_t subindex;
  uint8_t access; // 0 = RO, 1 = WO, 2 = RW
  uint8_t size;   // in bytes
  void* dataPtr;
} ODEntry;

ODEntry* findODEntry(uint16_t index, uint8_t subindex);

bool registerODEntry(uint16_t index, uint8_t subindex, uint8_t access, uint8_t size, void* dataPtr);

void initDefaultOD();

#endif