# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

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





