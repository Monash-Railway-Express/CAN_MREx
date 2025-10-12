#include <SPI.h>
#include <SD.h>

const int SD_CS = 26; // Change this to match your wiring

void setup() {
  Serial.begin(115200);
  delay(1000); // Give time for serial monitor to connect

  Serial.println("Initializing SD card...");

  // SPI.begin(18, 19, 23, SD_CS); // SCK, MISO, MOSI, CS

  if (!SD.begin(SD_CS)) {
    Serial.println("SD card initialization failed!");
    return;
  }
  Serial.println("SD card initialized.");

  // List files on SD card
  Serial.println("Listing files:");
  File root = SD.open("/");
  if (root) {
    listFiles(root, 0);
    root.close();
  } else {
    Serial.println("Failed to open root directory.");
  }

  // Write a test file
  File testFile = SD.open("/test.txt", FILE_WRITE);
  if (testFile) {
    testFile.println("SD card test successful!");
    testFile.close();
    Serial.println("Test file written.");
  } else {
    Serial.println("Failed to write test file.");
  }

  // Read back the test file
  testFile = SD.open("/test.txt");
  if (testFile) {
    Serial.println("Reading test file:");
    while (testFile.available()) {
      Serial.write(testFile.read());
    }
    testFile.close();
  } else {
    Serial.println("Failed to open test file.");
  }
}

void loop() {
  // Nothing here
}

void listFiles(File dir, int numTabs) {
  while (true) {
    File entry = dir.openNextFile();
    if (!entry) {
      break;
    }
    for (int i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      listFiles(entry, numTabs + 1);
    } else {
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}
