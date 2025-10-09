#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include "DFRobot_DS3231M.h"
#include "driver/twai.h"

const int SD_CS = 10; // chip select pin
DFRobot_DS3231M rtc; // rtc object
File logFile; // logfile
String logFilename; // filename

// Ring buffer config and instantiate
const int BUFFER_SIZE = 64;
struct CANFrame {
  String timestamp;
  uint32_t id;
  uint8_t dlc;
  uint8_t data[8];
};
CANFrame buffer[BUFFER_SIZE];
int bufferIndex = 0;
unsigned long lastFlush = 0;

//flush buffer function
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

void setup() {
  Serial.begin(115200);
  Wire.begin();

  // RTC init
  if (!rtc.begin()) {
    Serial.println("RTC not found");
    while (1);
  }

  //create file for logging to
  rtc.getNowTime();
  logFilename = "CAN_" +
                String(rtc.day()) + "-" +
                String(rtc.month()) + "-" +
                String(rtc.year()) + "_" +
                String(rtc.hour()) + "-" +
                String(rtc.minute()) + "-" +
                String(rtc.second()) + ".csv";

  // SD init
  if (!SD.begin(SD_CS)) {
    Serial.println("SD init failed");
    while (1);
  }

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
  if (twai_receive(&message, pdMS_TO_TICKS(5)) == ESP_OK) {
    rtc.getNowTime();
    CANFrame frame;
    frame.timestamp = String(rtc.year()) + "-" +
                      String(rtc.month()) + "-" +
                      String(rtc.day()) + " " +
                      String(rtc.hour()) + ":" +
                      String(rtc.minute()) + ":" +
                      String(rtc.second());
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
  if (millis() - lastFlush > 1000 && bufferIndex > 0) {
    flushBuffer();
  }
}
