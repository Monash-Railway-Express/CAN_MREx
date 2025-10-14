# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [2.0.0] - 2025-10-10
### Added
- New PyQt6 GUI layout with modular tabs and context-aware messaging
- Integrated CAN bus diagnostic overlay with real-time error feedback
- Support for belt presence detection using ultrasonic sensors

### Changed
- Refactored actuator control logic for improved ISR safety
- Updated dashboard integration to support Plotly and Dash

### Fixed
- EMI grounding issue on ESP32 board
- Version comment updater script now handles multi-line comments correctly

---

## [1.3.1] - 2025-09-15
### Fixed
- Logic analyzer misalignment in CAN stub detection
- GUI crash when switching tabs rapidly

---

## [1.3.0] - 2025-09-01
### Added
- Python script for bulk version comment updates in `.cpp` and `.h` files
- Initial dashboard prototype using Streamlit

### Changed
- Improved cable shielding recommendations in documentation

---

## [1.2.0] - 2025-08-10
### Added
- Inductive sensor evaluation module
- CSV log parser with BCE and Huber loss visualization

---

## [1.0.0] - 2025-07-01
### Added
- Initial release with train control logic, telemetry messaging, and GUI prototype