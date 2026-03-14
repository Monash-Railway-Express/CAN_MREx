# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [1.13.0] - 2026-
### Changed
- CAN is now a FreeRTOS task allowing you to pin it to a core, ensuring all messages are prioritied and therefore processed.
You must now add:
    xTaskCreatePinnedToCore(
        CAN_Task,
        "CAN Task",
        4096,
        &nodeID,   // <--- passed into pvParameters
        3,
        NULL,
        0
    );
To your Setup() after initCANMREX()
This has also created a clash where your nodeID declaration CANNOT be a const so change this line to:
    uint8_t nodeID = 3;
Also because of this feature you will now now no longer need to call handleCAN() so please get rid of any instance of this. This function will automatically be called by the FreeRTOS.

- Heart consumer now has a new API to turn it on or off. Put "enableHeartbeatMonitoring(true);" in the setup() to use it.

### Fixed
- Examples are now up to date

### Added
- Added tests for CAN MREx. This will mainly be for pre-release testing. It has basic tests for every function of CAN MREx.
- Added API to access buffer of emergencies. To see when emergency buffer has changed do:
if(checkMajorEMCY()) {
    ... insert code
}
checkMajorEMCY() / checkMinorEMCY()  will return true if buffer has changed.
To access a message in the buffer use:
bool getMinorByIndex(uint8_t index, uint8_t *node, uint32_t *code); // index you want | node the error came from | error code 
bool getMajorByIndex(uint8_t index, uint8_t *node, uint32_t *code); // index you want | node the error came from | error code 
Major emergencies have a buffer of 32 and the Minor emergencies have a buffer of 64
The index is in order from 0 being the newest message to max buffer - 1.

---

## [1.12.3] - 2026-3-9
- 1.12.2 failed to release

---

## [1.12.2] - 2026-3-6
### Changed
- Removed minor emergency overload

---

## [1.12.1] - 2026-1-13
### Changed
- small fixes and moved files

---

## [1.12.0] - 2026-1-13
### Changed
- The libary should now be added from the library manager rather than downloading and putting into your workspace
- The #include header has changed from #include <CM.h> to #include can_mrex.h

---

## [1.11.0] - 2025-10-10
### Added
- File version updater that updates all files in main in version control folder
- Added EMCY buffer
- EMCYS for NMT, EMCY, PDOs


### Changed
- Ensured all emergency messages will be processed by each node
- Made executeSDOReads return the value like value = executeSDORead()
- Removed size argument from SDOs and OD

---

## [1.10.0] - 2025-10-03
### Changes
CM 1.10.1 
- added headers so main.ino is cleaner
- added nmt master and heartbeat consumer variables for selecting those options

CM_ config 1.10.1
- added serial messages
-  moved intialisations here

EMCY 1.10.1
- Handle emergency function for major  emergencies 
- send emergency fucntion for sending them


CM_handler 1.10.1
- link to other functions
- link to SDO

CM_heartbeat 1.10.1
- send heartbeat function
- receive heartbeat function

CM_NMT 1.10.1
- handle NMT messages
- sendNMT messages

CM_ObjectDIctionary 1.10.1
- added operating mode

main.ino 1.10.1
- cleaning and organising layout

Created a new system for file Versions.

CM_Transmits became CM_SDO

CM_SDO 1.10.1
Added handleSDO function to this file from handler
Added EMCY fucntions

CM_EMCY 1.10.3
made it so changes own node operating mode to stopped when sending major emergency 

---

## [1.9.0] - 2025-09-09
### Added
- CM_Config (1.1.1)
- CM_Handler 1.1.3 - Handles TPDOS

### Improved
- CM_Transmit 1.1.3
- APIs for read and write requests
- Abstract away CAN stuff
- Main.ino 1.1.3
- much cleaner





