/**
 * CAN Logger file 
 *
 * File:            CAN_logger.ino
 * Organisation:    MREX
 * Author:          Chiara Gillam
 * Date Created:    12/10/2025
 * Last Modified:   12/10/2025
 * Version:         1.10.1
 *
 */



// Libraries used:
// https://wiki.dfrobot.com/DS3231M%20MEMS%20Precise%20RTC%20SKU:%20DFR0641



#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include "driver/twai.h"
#include "DFRobot_DS3231M.h"

// RTC
DFRobot_DS3231M rtc;

// SD card
const int SD_CS = 26;
File logFile;
String logFilename;

// Ring buffer config
const int BUFFER_SIZE = 32;
struct CANFrame {
  String timestamp;
  uint32_t id;
  uint8_t dlc;
  uint8_t data[8];
};
CANFrame buffer[BUFFER_SIZE];
int bufferIndex = 0;
unsigned long lastFlush = 0;

void setup() {
  Serial.begin(115200);
  Wire.begin();

  // RTC init
  while(rtc.begin() != true){
    Serial.println("Failed to init chip, please check if the chip connection is fine. ");
    delay(1000);
  }

    // SD init
  if (!SD.begin(SD_CS)) {
    Serial.println("SD init failed");
    while (1);
  }

  // File name init
  int testNumber = 0;
  char filename[13]; // 8.3 format: "/YYMMDDNN.CSV"
  bool fileExists = true;

  rtc.getNowTime();
  do {
    sprintf(filename, "/%02d%02d%02d%02d.CSV", rtc.year() % 100, rtc.month(), rtc.day(), testNumber);
    fileExists = SD.exists(filename);
    if (fileExists) testNumber++;
  } while (fileExists && testNumber < 100); // Limit to 00–99

  logFilename = String(filename);

  // Open log file and write header
  logFile = SD.open(logFilename, FILE_WRITE);
  if (logFile) {
    logFile.println("Timestamp,ID,DLC,Data0,Data1,Data2,Data3,Data4,Data5,Data6,Data7");
  } else {
    Serial.println("Failed to open log file");
    while (1);
  }

  // CAN init
  twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(GPIO_NUM_5, GPIO_NUM_4, TWAI_MODE_NORMAL);
  twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();
  twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

  if (twai_driver_install(&g_config, &t_config, &f_config) != ESP_OK ||
      twai_start() != ESP_OK) {
    Serial.println("CAN init failed");
    while (1);
  }
  Serial.println("CAN logging started");
}

void loop() {
  twai_message_t message;
  if (twai_receive(&message, pdMS_TO_TICKS(10)) == ESP_OK) {
    rtc.getNowTime();

    CANFrame frame;
    frame.timestamp = String(rtc.year()) + "-" +
                      String(rtc.month()) + "-" +
                      String(rtc.day()) + " " +
                      String(rtc.hour()) + ":" +
                      String(rtc.minute()) + ":" +
                      String(rtc.second());

    // // Fallback timestamp using millis
    // frame.timestamp = String(millis());

    frame.id = message.identifier;
    frame.dlc = message.data_length_code;
    for (int i = 0; i < 8; i++) {
      frame.data[i] = (i < message.data_length_code) ? message.data[i] : 0;
    }

    buffer[bufferIndex++] = frame;

    // Flush if buffer full
    if (bufferIndex >= BUFFER_SIZE) {
      flushBuffer();
    }
  }

  // Periodic flush
  if (millis() - lastFlush > 500 && bufferIndex > 0) {
    flushBuffer();
  }
}

void flushBuffer() {
  for (int i = 0; i < bufferIndex; i++) {
    logFile.print(buffer[i].timestamp);
    logFile.print(",0x");
    logFile.print(buffer[i].id, HEX);
    logFile.print(",");
    logFile.print(buffer[i].dlc);
    for (int j = 0; j < 8; j++) {
      logFile.print(",");
      if (j < buffer[i].dlc) {
        logFile.print("0x");
        logFile.print(buffer[i].data[j], HEX);
      }
    }
    logFile.println();
  }
  logFile.flush();
  bufferIndex = 0;
  lastFlush = millis();
}
