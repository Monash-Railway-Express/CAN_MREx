#include <Arduino.h>

// --- Pin Definitions ---
#define MOTOR_PIN GPIO_NUM_4
#define REGEN_BRAKE_PIN GPIO_NUM_5

// --- PWM Properties ---
const int freq = 5000;         // PWM frequency in Hz
const int resolution = 8;      // 8-bit resolution (0–255)
const int maxPWM = 255;        // Max duty cycle for 5V output
const int stepSize = 5;        // Step increment
const int stepDelay = 20;      // Delay between steps in ms

int pwmValue = 0;
bool rampUp = true;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("PWM ramp test starting...");

  // Attach PWM channels to pins
  ledcAttach(MOTOR_PIN, freq, resolution);
  ledcAttach(REGEN_BRAKE_PIN, freq, resolution);
}

void loop() {
  // Apply PWM to both pins
  ledcWrite(MOTOR_PIN, pwmValue);
  ledcWrite(REGEN_BRAKE_PIN, pwmValue);

  // Print debug info
  Serial.print("PWM Value: ");
  Serial.println(pwmValue);

  // Ramp logic
  if (rampUp) {
    pwmValue += stepSize;
    if (pwmValue >= maxPWM) {
      pwmValue = maxPWM;
      rampUp = false;
    }
  } else {
    pwmValue -= stepSize;
    if (pwmValue <= 0) {
      pwmValue = 0;
      rampUp = true;
    }
  }

  delay(stepDelay);
}
