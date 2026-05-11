#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "Voltmeter.h"
#include "Ammeter.h"
#include "Ohmmeter.h"

// ===== HARDWARE CONFIGURATION (Merged Constants) =====
#define PIN_VOLT A0
#define PIN_CURR A1
#define PIN_RES  A2

#define PIN_REF_LOW  6  
#define PIN_REF_MID  7
#define PIN_REF_HIGH 8 

#define BTN_VOLT 2
#define BTN_CURR 3
#define BTN_RES  4
#define BTN_PWR  5

const float VCC_REF = 5.0;      
const float INTERNAL_REF = 1.1; 
const int ADC_MAX = 1023;

// Global State
LiquidCrystal_I2C lcd(0x27, 16, 2);
enum Mode { IDLE, VOLTAGE, CURRENT, RESISTANCE, POWER };
Mode currentMode = IDLE;
Mode lastMode = IDLE;
int currentRange = 0; // 0:Low, 1:Mid, 2:High

unsigned long lastDisplayUpdate = 0;
const int displayInterval = 300; 

void setup() {
  pinMode(BTN_VOLT, INPUT_PULLUP);
  pinMode(BTN_CURR, INPUT_PULLUP);
  pinMode(BTN_RES,  INPUT_PULLUP);
  pinMode(BTN_PWR,  INPUT_PULLUP);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(3, 0);
  lcd.print("Smart Meter");
  delay(1500);
  lcd.clear();
}

void loop() {
  readButtons();

  if (millis() - lastDisplayUpdate > displayInterval) {
    if (currentMode != lastMode) {
      lcd.clear();
      lastMode = currentMode;
    }
    updateDisplay();
    lastDisplayUpdate = millis();
  }
}

void readButtons() {
  static unsigned long lastPress = 0;
  if (millis() - lastPress < 250) return;

  if (digitalRead(BTN_VOLT) == LOW) { currentMode = VOLTAGE; lastPress = millis(); }
  else if (digitalRead(BTN_CURR) == LOW) { currentMode = CURRENT; lastPress = millis(); }
  else if (digitalRead(BTN_RES) == LOW) { 
    if (currentMode == RESISTANCE) {
      currentRange = (currentRange + 1) % 3;
    }
    currentMode = RESISTANCE; lastPress = millis(); 
  }
  else if (digitalRead(BTN_PWR) == LOW) { currentMode = POWER; lastPress = millis(); }
}

// Global ADC helper used by all modules
float readADC(int pin, int samples) {
  long sum = 0;
  for (int i = 0; i < samples; i++) { sum += analogRead(pin); }
  return (float)sum / samples;
}

void updateDisplay() {
  switch (currentMode) {
    case VOLTAGE:
      displayVoltage(getVoltage());
      break;
    case CURRENT:
      displayCurrent(getCurrent());
      break;
    case POWER:
      displayPower(getVoltage(), getCurrent());
      break;
    case RESISTANCE:
      runOhmmeter(currentRange);
      break;
    default:
      lcd.setCursor(0, 0); lcd.print("Select Mode:");
      break;
  }
}
