/* framehandler.cpp
 *
 * Library to read from Victron devices using VE.Direct protocol.
 * Derived from Victron framehandler reference implementation.
 *
 * The MIT License
 *
 * Copyright (c) 2019 Victron Energy BV
 * Portions Copyright (C) 2020 Chris Terwilliger
 * Portions Copyright (C) 2022 Martin Verges
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * 2020.05.05 - 0.2 - initial release
 * 2020.06.21 - 0.2 - add MIT license, no code changes
 * 2020.08.20 - 0.3 - corrected #include reference
 * 2022.05.16 - 0.4 - drop arduino requirements, add cmake, add pedantic, chksum without \t\n\s
 * 2022.05.17 - 0.5 - add support for the HEX protocol
 */
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// include the frame handler header
#include "VeDirectFrameHandler.h"
// allows to enable or disable printing for debugging without editing the source file
#ifndef DEBUG_MODE
#define DEBUG_MODE false
#endif

static constexpr char checksumTagName[] = "CHECKSUM";

/**
 * @brief Construct a new Ve Direct Frame Handler:: Ve Direct Frame Handler object
 */
VeDirectFrameHandler::VeDirectFrameHandler() {}
/**
 * @brief Return true if new data can be read
 * 
 * @return true on new data
 * @return false when no data available
 */
// returns true if a valid complete frame is received and copied into veData
bool VeDirectFrameHandler::isDataAvailable() {
  return newDataAvailable;
}

/**
 * @brief Clear state and wait for new data
 */
// clear the flag for the next frame
void VeDirectFrameHandler::clearData() {
  newDataAvailable = false;
}

/**
 * @brief Reads the next byte given and parses it into the frame
 * @details This function is called by the application which passes a byte of serial data
 *
 * @param inbyte Input byte to store in the tmp memory
 */
void VeDirectFrameHandler::rxData(uint8_t inbyte) {
    // if debug mode is enabled then print the old checksum before adding the new byte
    if(DEBUG_MODE) printf("Checksum = %3d ", mChecksum);
    // add each byte to the running sum
    mChecksum += inbyte;
    // if debug mode is enabled then print the new checksum after adding the new byte
    // print the incoming byte and the checksum after adding it as well as the current state
    if(DEBUG_MODE) {
      if (isprint(inbyte))      printf("+ %3d (%c)  ", inbyte, inbyte);
      else if (inbyte == '\n')  printf("+ %3d (\\n) ", inbyte);
      else if (inbyte == '\r')  printf("+ %3d (\\r) ", inbyte);
      else if (inbyte == '\t')  printf("+ %3d (\\t) ", inbyte);
      else printf("+ %3d (.)  ", inbyte);
      printf("= %3d  ", mChecksum);
      printf("mState = %d\n", mState);
    }
  
  switch(mState) {
    case IDLE:
      // Wait for \n of the start of a record
      switch(inbyte) {
        case '\n':
          if(DEBUG_MODE) printf("Transition from IDLE to RECORD_BEGIN\n");
          mState = RECORD_BEGIN;
          break;
        default: // skip \r and incomplete line data
          break;
      }
      break;
    case RECORD_BEGIN:
      // Start the record of the label name
      mTextPointer = mName;
      *mTextPointer++ = inbyte;
      mState = RECORD_NAME;
      break;
    case RECORD_NAME:
      // The record name is being received, terminated by a \t
      switch(inbyte) {
        case '\t': // End of field name (seperator)
          // the Checksum record indicates a EOR
          if (mTextPointer < (mName + sizeof(mName))) {
            *mTextPointer = 0; // Zero terminate
            if (strcmp(mName, checksumTagName) == 0) {
              mState = CHECKSUM;
              break;
            }
          }
          mTextPointer = mValue; // Reset value pointer to record the value
          mState = RECORD_VALUE;
          break;
        default:
          // keep recording name of field
          if (mTextPointer < (mName + sizeof(mName)-1))
              *mTextPointer++ = toupper(inbyte);
          break;
      }
      break;
    case RECORD_VALUE:
      // The record value is being received. The \n indicates a new record.
      switch(inbyte) {
        case '\n':
          if (mTextPointer < (mValue + sizeof(mValue))) {
            *mTextPointer = 0; // make zero ended
            textRxEvent(mName, mValue); // call textRxEvent to store the name-value pair in temporary buffer
          }
          mState = RECORD_BEGIN;
          break;
        case '\r': // Ignore
          break;
        default:
          // keep recording value
          if (mTextPointer < (mValue + sizeof(mValue)-1))
            *mTextPointer++ = inbyte;
          break;
      }
      break;
    case CHECKSUM:
      if (mChecksum != 0) printf("[CHECKSUM] Invalid frame - checksum is %d\n", mChecksum);
      mState = IDLE;
      frameEndEvent(ignoreCheckSum || mChecksum == 0); // call frameEndEvent to store the name-value pair in public buffer
      mChecksum = 0;
      break;
  } // End of switch(mState)
}

/**
 * @brief This function is called every time a new name/value is successfully parsed.  It writes the values to the temporary buffer.
 *
 * @param mName     Name of the field
 * @param mValue    Value of the field
 */
void VeDirectFrameHandler::textRxEvent(char * mName, char * mValue) {
  if (frameIndex < blockLen) {                       
    strcpy(tempData[frameIndex].veName, mName);      // copy name to temporary buffer
    strcpy(tempData[frameIndex].veValue, mValue);    // copy value to temporary buffer
    frameIndex++;
  }
}

/**
 * @brief This function is called at the end of the received frame.
 * @details If the checksum is valid, the temp buffer is read line by line.
 *          If the name exists in the public buffer, the new value is copied to the public buffer.
 *          If not, a new name/value entry is created in the public buffer.
 *
 * @param valid Set to true if the checksum was correct
 */
void VeDirectFrameHandler::frameEndEvent(bool valid) {
  if (valid) {
    newDataAvailable = true;
    // check if there is an existing entry
    for (int i = 0; i < frameIndex; i++) {                  // read each name already in the temp buffer
      bool nameExists = false;
      for (int j = 0; j <= veEnd; j++) {                    // compare to existing names in the public buffer
        if (strcmp(tempData[i].veName, veData[j].veName) == 0) {
          strcpy(veData[j].veValue, tempData[i].veValue);   // overwrite tempValue in the public buffer
          nameExists = true;
          break;
        }
      }
      if (!nameExists) {
        strcpy(veData[veEnd].veName, tempData[i].veName);   // write new Name to public buffer
        strcpy(veData[veEnd].veValue, tempData[i].veValue); // write new Value to public buffer
        veEnd++;                                            // increment end of public buffer
        if (veEnd >= buffLen) veEnd = buffLen - 1;          // stop any buffer overrun
      }
    }
  }
  frameIndex = 0;    // reset frame
}

/*int findRecoveredEnergy(){
if (myparser.isDataAvailable(){ // if there is data available in the buffer. Now "new" data should be available every second.
  for (int i = 0; i < myparser.veEnd; ++i) { // go through the buffer 
  if (strcmp(myparser.veData[i].veName, "P") == 0){ // extract power
  power = atoi(myparser.veData[i].veValue); 
  if(power < 0){
  for (int i = 0; i < regen_brake_duration ; ++i){
  latest_power_value = -power; // Value in watts W. Just take the absolute value.
  slice = ((lastest_power_value + prev_power_value)/2)*1 // the shunt sends a new block every second so the time between new power samples is 1 s.
  recovered_energy += slice; // keep accumulating
  prev_power_value = latest_power_value;
  if (power == 0){
  time_regen_brake_end = millis(); // if power is 0, that is when regen ends
  }
  regen_brake_duration = time_regen_brake_end - time_regen_brake_start;
} 
}
}
return recovered_energy;
}
*/
