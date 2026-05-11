#include <Wire.h>
#include <LiquidCrystal_I2C.h>

/* * ============================================================================
 * 1. SYSTEM CONSTANTS & PIN DEFINITIONS
 * ============================================================================
 */
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

// Calibration & Hardware Constants
const float VCC_REF = 5.0;      
const float INTERNAL_REF = 1.1; 
const int ADC_MAX = 1023;

const float R_DIV_TOP = 99300.0;
const float R_DIV_BOTTOM = 9780.0;

const float R_SHUNT = 1.11;
const float AMP_GAIN = 1.0; 

const float R_REF_LOW  = 1000.0;
const float R_REF_MID  = 9800.0;
const float R_REF_HIGH = 98000.0;

// LCD Initialization
LiquidCrystal_I2C lcd(0x27, 16, 2);

// State Variables
enum Mode { IDLE, VOLTAGE, CURRENT, RESISTANCE, POWER };
Mode currentMode = IDLE;
Mode lastMode = IDLE;

enum ResRange { RANGE_LOW, RANGE_MID, RANGE_HIGH };
ResRange currentRange = RANGE_LOW;

unsigned long lastDisplayUpdate = 0;
const int displayInterval = 300; 

// Calibration Multipliers
float voltCal = 1.036;
float currCal = 1.023;
float resCal  = 1.025;

// Global Filter State
float filteredCurrent = 0.0;

/* * ============================================================================
 * 2. VOLTMETER MODULE
 * ============================================================================
 */
float getVoltage() {
  analogReference(DEFAULT); 
  delay(15); 
  
  float adc = readADC(PIN_VOLT, 30);
  float vOut = (adc * VCC_REF) / ADC_MAX;
  float finalV = vOut * ((R_DIV_TOP + R_DIV_BOTTOM) / R_DIV_BOTTOM) * voltCal;
  
  return (finalV < 0.05) ? 0.0 : finalV; 
}

/* * ============================================================================
 * 3. AMMETER MODULE (with EMA Filter)
 * ============================================================================
 */
float getCurrent() {
  analogReference(INTERNAL); 
  delay(15); 
  
  float adc_avg = readADC(PIN_CURR, 200); 
  float vSense = (adc_avg * INTERNAL_REF) / ADC_MAX;
  float rawCurrent = (vSense / (R_SHUNT * AMP_GAIN)) * currCal;

  // Range-specific error correction (16% boost for low currents)
  float correctedCurrent = (rawCurrent < 0.090) ? (rawCurrent * 1.16) : rawCurrent;

  // Fast-Jump EMA Filter Logic
  float diff = abs(correctedCurrent - filteredCurrent);
  if (diff > 0.030) { 
    filteredCurrent = correctedCurrent; // High responsiveness for large changes
  } else {
    float alpha = (rawCurrent < 0.05) ? 0.15 : 0.3; // High stability for small signals
    filteredCurrent = (alpha * correctedCurrent) + (1.0 - alpha) * filteredCurrent;
  }

  return (filteredCurrent < 0.001) ? 0.0 : filteredCurrent;
}

/* * ============================================================================
 * 4. OHMMETER MODULE (Multi-Range)
 * ============================================================================
 */
void showResistance() {
  analogReference(DEFAULT); 
  delay(15);
  
  float activeRef;
  switch (currentRange) {
    case RANGE_LOW:
      pinMode(PIN_REF_MID, INPUT); pinMode(PIN_REF_HIGH, INPUT);
      pinMode(PIN_REF_LOW, OUTPUT); digitalWrite(PIN_REF_LOW, HIGH);
      activeRef = R_REF_LOW;
      lcd.setCursor(0, 0); lcd.print("Res [Low 1k]:   ");
      break;
    case RANGE_MID:
      pinMode(PIN_REF_LOW, INPUT); pinMode(PIN_REF_HIGH, INPUT);
      pinMode(PIN_REF_MID, OUTPUT); digitalWrite(PIN_REF_MID, HIGH);
      activeRef = R_REF_MID;
      lcd.setCursor(0, 0); lcd.print("Res [Mid 10k]:  ");
      break;
    case RANGE_HIGH:
      pinMode(PIN_REF_LOW, INPUT); pinMode(PIN_REF_MID, INPUT);
      pinMode(PIN_REF_HIGH, OUTPUT); digitalWrite(PIN_REF_HIGH, HIGH);
      activeRef = R_REF_HIGH;
      lcd.setCursor(0, 0); lcd.print("Res [High 100k]:");
      break;
  }

  delay(10);
  float adc = readADC(PIN_RES, 30);
  if (adc >= 1015) { lcd.setCursor(0, 1); lcd.print("OL (Open)        "); return; }
  
  float vOut = (adc * VCC_REF) / ADC_MAX;
  float rCalc = activeRef * (vOut / (VCC_REF - vOut)) * resCal;

  lcd.setCursor(0, 1);
  if (rCalc < 1000) { lcd.print(rCalc, 1); lcd.print(" Ohm      "); }
  else if (rCalc < 1000000) { lcd.print(rCalc / 1000.0, 2); lcd.print(" kOhm     "); }
  else { lcd.print(rCalc / 1000000.0, 2); lcd.print(" MOhm     "); }
}

/* * ============================================================================
 * 5. MAIN CORE LOGIC
 * ============================================================================
 */
void setup() {
  pinMode(BTN_VOLT, INPUT_PULLUP);
  pinMode(BTN_CURR, INPUT_PULLUP);
  pinMode(BTN_RES,  INPUT_PULLUP);
  pinMode(BTN_PWR,  INPUT_PULLUP);

  pinMode(PIN_REF_LOW,  INPUT);
  pinMode(PIN_REF_MID,  INPUT);
  pinMode(PIN_REF_HIGH, INPUT);

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
      if (currentRange == RANGE_LOW) currentRange = RANGE_MID;
      else if (currentRange == RANGE_MID) currentRange = RANGE_HIGH;
      else currentRange = RANGE_LOW;
    }
    currentMode = RESISTANCE; lastPress = millis(); 
  }
  else if (digitalRead(BTN_PWR) == LOW) { currentMode = POWER; lastPress = millis(); }
}

float readADC(int pin, int samples) {
  long sum = 0;
  for (int i = 0; i < samples; i++) { 
    sum += analogRead(pin);
  }
  return (float)sum / samples;
}

void updateDisplay() {
  switch (currentMode) {
    case VOLTAGE:
      lcd.setCursor(0, 0); lcd.print("Mode: Voltage   ");
      lcd.setCursor(0, 1); lcd.print(getVoltage(), 2); lcd.print(" V         ");
      break;

    case CURRENT: {
      float i = getCurrent();
      lcd.setCursor(0, 0); lcd.print("Mode: Current   ");
      lcd.setCursor(0, 1); 
      if (i < 1.0) { lcd.print(i * 1000.0, 1); lcd.print(" mA        "); }
      else { lcd.print(i, 3); lcd.print(" A         "); }
      break;
    }

    case POWER: {
      float v = getVoltage();
      float i = getCurrent();
      lcd.setCursor(0, 0); lcd.print("Mode: Power     ");
      lcd.setCursor(0, 1); lcd.print(v * i, 2); lcd.print(" W         ");
      break;
    }

    case RESISTANCE:
      showResistance();
      break;

    default:
      lcd.setCursor(0, 0); lcd.print("Select Mode:    ");
      lcd.setCursor(0, 1); lcd.print("V | A | R | W   ");
      break;
  }
}
