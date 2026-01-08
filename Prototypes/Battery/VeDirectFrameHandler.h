/* frameHandler.h
 *
 * Library to read from Victron devices using VE.Direct protocol.
 * Derived from Victron framehandler reference implementation.
 * 
 * 2020.05.05 - 0.2 - initial release
 * 2021.02.23 - 0.3 - change frameLen to 22 per VE.Direct Protocol version 3.30
 * 2022.05.16 - 0.4 - drop arduino requirements, by Martin Verges
 * 2022.05.17 - 0.5 - add support for the HEX protocol
 */
#include <stdint.h>
#ifndef FRAMEHANDLER_H_
#define FRAMEHANDLER_H_

const uint8_t blockLen = 22;        // VE.Direct Protocol: maximum number of fields in a block is 22. 1 block sent per second. Each block has 22 fields max so block length is 22
const uint8_t labelLen = 9;         // VE.Direct Protocol: for field label, a buffer of 9 bytes is needed. 8 chars max + null terminator.
const uint8_t valueLen = 33;        // VE.Direct Protocol: for field value, a buffer of 33 bytes is needed.
const uint8_t buffLen = 22;         // maximum number of fields for the smart shunt is 22.

class VeDirectFrameHandler {
  public:
    VeDirectFrameHandler();

    void rxData(uint8_t inbyte); // each byte from shunt is passed into this function
    bool isDataAvailable(); // returns true if a block was received and stored in veData
    void clearData(); // clears the public buffer so the next block can be stored
    
    // this struct holds the name/value pair received from the VE.Direct interface. This is the line that we are currently reading.
    // value should be parsed as soon as a single field is received and kept in a temporary record
    struct VeData {
      char veName[labelLen];
      char veValue[valueLen];
    };

    VeData veData[buffLen] = { };               // public buffer for received text frames. This buffer stores the frame after the checksum was verified.
    bool newDataAvailable = false;              // will be set to true after receiving a full valid frame 

    int frameIndex = 0;                         // which line of the frame are we parsing atm
    int veEnd = 0;                              // current size (end) of the public buffer

    bool ignoreCheckSum = false;                // Disable checksum verification. If set to true, all frames are accepted regardless of checksum validity.

  // private methods and variables. The final buffer with all the verified data is public.
  private:
    enum States {                               // state machine
      IDLE,
      RECORD_BEGIN,
      RECORD_NAME,
      RECORD_VALUE,
      CHECKSUM,
    };

    int mState = States::IDLE;                  // current state
    uint8_t mChecksum = 0;                      // checksum value
    char * mTextPointer;                        // pointer to the private buffer we're writing to, name or value

    VeData tempData[blockLen];                         // private/temporary buffer for current frame as it is parsed. Once checksum is verified, frame copied to public buffer
    char mName[labelLen];                              // buffer for the current field's name
    char mValue[valueLen];                            // buffer for the current field's value

    void textRxEvent(char *, char *); // called after a line is read. stores name/value pair in temporary buffer
    void frameEndEvent(bool);  // called at the end of the frame after checksum verification. Moves data from temporary buffer to public buffer.

};

#endif // FRAMEHANDLER_H_

