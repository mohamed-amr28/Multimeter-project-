# ⚡ Arduino Multi-Meter — Voltmeter | Ammeter | Ohmmeter | Wattmeter

<p align="center">
  <img src="docs/schematic.png" alt="Multi-Meter Circuit Schematic" width="900"/>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/Platform-Arduino%20UNO-00979D?style=for-the-badge&logo=arduino&logoColor=white"/>
  <img src="https://img.shields.io/badge/Language-C%2B%2B%20%2F%20Arduino-blue?style=for-the-badge"/>
  <img src="https://img.shields.io/badge/Display-I2C%20LCD%2016x2-green?style=for-the-badge"/>
  <img src="https://img.shields.io/badge/License-MIT-yellow?style=for-the-badge"/>
</p>

---

## 📋 Table of Contents

- [Overview](#-overview)
- [Features](#-features)
- [Hardware Requirements](#-hardware-requirements)
- [Circuit Connections](#-circuit-connections)
  - [LCD (I2C)](#1-i2c-lcd-16x2)
  - [Voltmeter](#2-voltmeter-connection)
  - [Ammeter](#3-ammeter-connection-shunt-method)
  - [Ohmmeter](#4-ohmmeter-connection)
  - [Wattmeter](#5-wattmeter-connection)
- [Working Principle](#-working-principle)
- [Component Values](#-component-values)
- [Calibration](#-calibration)
- [Measurement Accuracy](#-measurement-accuracy)
- [Getting Started](#-getting-started)
- [Full Source Code](#-full-source-code)
- [Team](#-team)

---

## 🔍 Overview

This project implements a **four-mode digital measurement instrument** using an **Arduino UNO** microcontroller. The system can function as a **Voltmeter**, **Ammeter**, **Ohmmeter**, or **Wattmeter**, with the mode selected via dedicated push buttons. All readings are displayed on a **16×2 I2C LCD**.

The design applies fundamental electrical laws — **Ohm's Law**, **Kirchhoff's Voltage Law**, and the **Voltage Divider Rule** — to translate real-world electrical quantities into digital readings via the Arduino's 10-bit ADC (0–1023 counts over 0–5 V).

---

## ✨ Features

| Feature | Details |
|---|---|
| 🔋 Voltage Measurement | Up to **~25 V DC** via resistive voltage divider |
| ⚡ Current Measurement | Up to **5 A** via shunt resistor method |
| 🔩 Resistance Measurement | Three auto-selectable ranges: **0–2 kΩ**, **1–20 kΩ**, **10–200 kΩ** |
| 💡 Power Measurement | Instantaneous DC power (**P = V × I**) |
| 🖥️ Display | I2C 16×2 LCD — minimal wiring (only 2 signal wires) |
| 🎛️ Mode Selection | 4 dedicated push buttons (D2–D5) |
| 📐 ADC Averaging | Multi-sample averaging for stable readings |
| 🔒 Open-Circuit Detection | Ohmmeter shows `OL (Open)` when no resistor connected |

---

## 🛒 Hardware Requirements

| Component | Value / Model | Qty | Purpose |
|---|---|---|---|
| Arduino UNO | ATmega328 | 1 | Main microcontroller |
| LCD 16×2 (I2C) | HD44780 + PCF8574 | 1 | Display readings |
| Resistor R1 | 99.3 kΩ | 1 | Voltage divider upper arm |
| Resistor R2 | 9.78 kΩ | 1 | Voltage divider lower arm |
| Shunt Resistor | 0.1 Ω | 1 | Current sensing |
| Resistor R_LOW | 1 kΩ, 1% tolerance | 1 | Ohmmeter low range |
| Resistor R_MID | 9.8 kΩ, 1% tolerance | 1 | Ohmmeter mid range |
| Resistor R_HIGH | 98 kΩ, 1% tolerance | 1 | Ohmmeter high range |
| Push Buttons | Momentary SPST | 4 | Mode selection (D2–D5) |
| Breadboard + Wires | — | 1 set | Prototyping |
| Power Supply | 0–25 V DC | 1 | Voltage source under test |

---

## 🔌 Circuit Connections

### 1. I2C LCD 16×2

| LCD Pin | Arduino Pin |
|---|---|
| VCC | 5V |
| GND | GND |
| SDA | A4 |
| SCL | A5 |

---

### 2. Voltmeter Connection

```
VIN (+) ────┬──── R1 (99.3 kΩ) ────┬──── VIN (−) / GND
            │                       │
            │                    R2 (9.78 kΩ)
            │                       │
            │                  ├──── Arduino A0 (ADC input)
            │                       │
            └───────────────────── GND
```

> **Note:** The junction between R1 and R2 connects to ADC pin **A0**.

---

### 3. Ammeter Connection (Shunt Method)

```
SOURCE (+) ────── LOAD ────── RSHUNT (0.1 Ω) ────── SOURCE (−)
                                    │
                              Arduino A1
```

> **Note:** Arduino reads the voltage across the shunt resistor at **A1**.

---

### 4. Ohmmeter Connection

```
Arduino D6 (HIGH) ──── R1 (1 kΩ)  ──────┬──── Arduino A2
Arduino D7 (HIGH) ──── R2 (9.8 kΩ) ─────┤
Arduino D8 (HIGH) ──── R3 (98 kΩ)  ──────┤
                                          │
                                        Rx (Unknown Resistor)
                                          │
                                        GND
```

> **Only one digital pin is driven HIGH at a time** to select the measurement range.

---

### 5. Wattmeter Connection

```
SOURCE (+) ──┬── RSHUNT ──── LOAD (+) ──── LOAD (−) ──── SOURCE (−)
             │       │
          R1 (99.3 kΩ)  └──── Arduino A1 (Current)
             │
          R2 (9.78 kΩ) ──── Arduino A0 (Voltage)
             │
           GND
```

> **Power = Voltage × Current** — both sub-circuits operate simultaneously.

---

### Mode Selection Buttons

| Button | Arduino Pin | Mode |
|---|---|---|
| VOLTAGE | D2 | Voltmeter |
| CURRENT | D3 | Ammeter |
| RESISTANCE | D4 | Ohmmeter (press again to cycle range) |
| POWER | D5 | Wattmeter |

---

## 🧮 Working Principle

### Voltmeter — Voltage Divider Rule
```
V_out = V_in × R2 / (R1 + R2)
V_in  = V_adc × (R1 + R2) / R2
V_max ≈ 25 V  (with R1 = 99.3 kΩ, R2 = 9.78 kΩ)
```

### Ammeter — Shunt Resistor (Ohm's Law)
```
I = V_shunt / R_shunt
```
Arduino reads the voltage drop across R_SHUNT = 0.1 Ω at pin **A1**.

### Ohmmeter — Voltage Divider Rule
```
Rx = R_ref × V_out / (VCC − V_out)
```
Three reference resistors (1 kΩ / 9.8 kΩ / 98 kΩ) cover three measurement ranges.

### Wattmeter — Power Law (Joule)
```
P = V × I   [Watts]
```
Instantaneous DC power computed by combining voltage and current readings.

---

## 📊 Component Values

| Parameter | Symbol | Value | Unit |
|---|---|---|---|
| ADC Supply Voltage | VCC_REF | 5.0 | V |
| ADC Resolution | ADC_MAX | 1023 | counts |
| Voltage Divider Upper | R1 | 99,300 | Ω |
| Voltage Divider Lower | R2 | 9,780 | Ω |
| Shunt Resistor | R_SHUNT | 0.1 | Ω |
| Ohm Ref Low | R_REF_LOW | 1,000 | Ω |
| Ohm Ref Mid | R_REF_MID | 9,800 | Ω |
| Ohm Ref High | R_REF_HIGH | 98,000 | Ω |
| Resistance Calibration | resCal | 1.025 | — |

---

## 🎯 Calibration

### Voltmeter
Apply a known reference voltage (e.g. 12.00 V). If display reads 11.85 V, correction factor = `12.00 / 11.85 = 1.0127`. Adjust `R1`/`R2` constants in code accordingly.

### Ammeter
- **Zero cal:** With no current, ADC on A1 should give ~512 counts (2.5 V equivalent via shunt). Adjust `R_SHUNT` constant if needed.
- **Gain cal:** Apply a known current (e.g. 1.000 A) and adjust `currCal` multiplier.

### Ohmmeter
Connect a precision resistor (e.g. 10.00 kΩ, 0.1%). Set `resCal = R_known / R_displayed`. Default is `1.0`, typical range is `0.95–1.05`.

### Wattmeter
Calibrate voltage and current sub-circuits independently, then verify with a known load (e.g. 5 V × 0.5 A → 2.5 W expected).

---

## 📏 Measurement Accuracy

| Mode | Practical Error |
|---|---|
| Voltmeter | 0.75 % |
| Ammeter | 1.25 % |
| Ohmmeter (Low range) | 2.2 % |
| Ohmmeter (Mid range) | 0.3 % |
| Ohmmeter (High range) | 1.0 % |
| Wattmeter | 4.0 % |

---

## 🚀 Getting Started

### Prerequisites

- [Arduino IDE](https://www.arduino.cc/en/software) (v1.8+ or v2.x)
- Library: `LiquidCrystal_I2C` — install via Arduino Library Manager

### Installation

```bash
# 1. Clone the repository
git clone https://github.com/YOUR_USERNAME/arduino-multimeter.git

# 2. Open the sketch
cd arduino-multimeter
# Open MultiMeter.ino in Arduino IDE

# 3. Install library
# Arduino IDE → Tools → Manage Libraries → Search "LiquidCrystal I2C" → Install

# 4. Upload
# Select Board: Arduino UNO
# Select Port, then click Upload
```

### Library Setup

```cpp
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);  // Change 0x27 to 0x3F if LCD doesn't show
```

> **Tip:** If the LCD address `0x27` doesn't work, run an [I2C Scanner sketch](https://playground.arduino.cc/Main/I2cScanner/) to find your module's address.

---

## 💻 Full Source Code

<details>
<summary><b>Click to expand full Arduino sketch</b></summary>

```cpp
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// ===== PINS =====
#define PIN_VOLT    A0
#define PIN_CURR    A1
#define PIN_RES     A2
#define PIN_REF_LOW  6
#define PIN_REF_MID  7
#define PIN_REF_HIGH 8
#define BTN_VOLT 2
#define BTN_CURR 3
#define BTN_RES  4
#define BTN_PWR  5

// ===== CONSTANTS =====
const float VCC_REF      = 5.0;
const int   ADC_MAX      = 1023;
const float R_DIV_TOP    = 99300.0;
const float R_DIV_BOTTOM =  9780.0;
const float R_SHUNT      =  1.11;
const float AMP_GAIN     =  1.0;
const float R_REF_LOW    =  1000.0;
const float R_REF_MID    =  9800.0;
const float R_REF_HIGH   = 98000.0;

// ===== CALIBRATION =====
float voltCal = 1.036;
float currCal = 1.023;
float resCal  = 1.025;

// ===== FILTER =====
float filteredCurrent = 0.0;

// ===== STATE =====
enum Mode     { IDLE, VOLTAGE, CURRENT, RESISTANCE, POWER };
enum ResRange { RANGE_LOW, RANGE_MID, RANGE_HIGH };

Mode     currentMode  = IDLE;
Mode     lastMode     = IDLE;
ResRange currentRange = RANGE_LOW;

unsigned long lastDisplayUpdate = 0;
const int displayInterval = 300;

// ===== SETUP =====
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

// ===== MAIN LOOP =====
void loop() {
  readButtons();
  if (millis() - lastDisplayUpdate > displayInterval) {
    if (currentMode != lastMode) { lcd.clear(); lastMode = currentMode; }
    updateDisplay();
    lastDisplayUpdate = millis();
  }
}

// ===== BUTTONS =====
void readButtons() {
  static unsigned long lastPress = 0;
  if (millis() - lastPress < 250) return;

  if      (digitalRead(BTN_VOLT) == LOW) { currentMode = VOLTAGE;    lastPress = millis(); }
  else if (digitalRead(BTN_CURR) == LOW) { currentMode = CURRENT;    lastPress = millis(); }
  else if (digitalRead(BTN_RES)  == LOW) {
    if (currentMode == RESISTANCE) {
      if      (currentRange == RANGE_LOW)  currentRange = RANGE_MID;
      else if (currentRange == RANGE_MID)  currentRange = RANGE_HIGH;
      else                                 currentRange = RANGE_LOW;
    }
    currentMode = RESISTANCE; lastPress = millis();
  }
  else if (digitalRead(BTN_PWR)  == LOW) { currentMode = POWER;      lastPress = millis(); }
}

// ===== ADC AVERAGING =====
float readADC(int pin, int samples) {
  long sum = 0;
  for (int i = 0; i < samples; i++) sum += analogRead(pin);
  return (float)sum / samples;
}

// ===== VOLTAGE =====
float getVoltage() {
  analogReference(DEFAULT); delay(15);
  float adc  = readADC(PIN_VOLT, 30);
  float vOut = (adc * VCC_REF) / ADC_MAX;
  float finalV = vOut * ((R_DIV_TOP + R_DIV_BOTTOM) / R_DIV_BOTTOM) * voltCal;
  return (finalV < 0.05) ? 0.0 : finalV;
}

// ===== CURRENT =====
float getCurrent() {
  analogReference(INTERNAL); delay(15);
  float adc_avg    = readADC(PIN_CURR, 200);
  float vSense     = (adc_avg * 1.1) / ADC_MAX;
  float rawCurrent = (vSense / (R_SHUNT * AMP_GAIN)) * currCal;

  float correctedCurrent = (rawCurrent < 0.090) ? rawCurrent * 1.16 : rawCurrent;

  float diff = abs(correctedCurrent - filteredCurrent);
  if (diff > 0.030) {
    filteredCurrent = correctedCurrent;
  } else {
    float alpha = (rawCurrent < 0.05) ? 0.15 : 0.3;
    filteredCurrent = (alpha * correctedCurrent) + (1.0 - alpha) * filteredCurrent;
  }
  return (filteredCurrent < 0.001) ? 0.0 : filteredCurrent;
}

// ===== DISPLAY =====
void updateDisplay() {
  switch (currentMode) {
    case VOLTAGE:
      lcd.setCursor(0, 0); lcd.print("Mode: Voltage ");
      lcd.setCursor(0, 1); lcd.print(getVoltage(), 2); lcd.print(" V   ");
      break;

    case CURRENT: {
      float i = getCurrent();
      lcd.setCursor(0, 0); lcd.print("Mode: Current ");
      lcd.setCursor(0, 1);
      if (i < 1.0) { lcd.print(i * 1000.0, 1); lcd.print(" mA  "); }
      else          { lcd.print(i, 3);           lcd.print(" A   "); }
      break;
    }

    case POWER: {
      float v = getVoltage(), i = getCurrent();
      lcd.setCursor(0, 0); lcd.print("Mode: Power   ");
      lcd.setCursor(0, 1); lcd.print(v * i, 2); lcd.print(" W   ");
      break;
    }

    case RESISTANCE:
      showResistance();
      break;

    default:
      lcd.setCursor(0, 0); lcd.print("Select Mode:  ");
      lcd.setCursor(0, 1); lcd.print("V | A | R | W ");
      break;
  }
}

// ===== RESISTANCE =====
void showResistance() {
  analogReference(DEFAULT); delay(15);
  float activeRef;

  switch (currentRange) {
    case RANGE_LOW:
      pinMode(PIN_REF_MID, INPUT); pinMode(PIN_REF_HIGH, INPUT);
      pinMode(PIN_REF_LOW, OUTPUT); digitalWrite(PIN_REF_LOW, HIGH);
      activeRef = R_REF_LOW;
      lcd.setCursor(0, 0); lcd.print("Res [Low  1k]:");
      break;
    case RANGE_MID:
      pinMode(PIN_REF_LOW, INPUT); pinMode(PIN_REF_HIGH, INPUT);
      pinMode(PIN_REF_MID, OUTPUT); digitalWrite(PIN_REF_MID, HIGH);
      activeRef = R_REF_MID;
      lcd.setCursor(0, 0); lcd.print("Res [Mid 10k]:");
      break;
    case RANGE_HIGH:
      pinMode(PIN_REF_LOW, INPUT); pinMode(PIN_REF_MID, INPUT);
      pinMode(PIN_REF_HIGH, OUTPUT); digitalWrite(PIN_REF_HIGH, HIGH);
      activeRef = R_REF_HIGH;
      lcd.setCursor(0, 0); lcd.print("Res [Hi 100k]:");
      break;
  }

  delay(10);
  float adc = readADC(PIN_RES, 30);
  if (adc >= 1015) { lcd.setCursor(0, 1); lcd.print("OL (Open)     "); return; }

  float vOut  = (adc * VCC_REF) / ADC_MAX;
  float rCalc = activeRef * (vOut / (VCC_REF - vOut)) * resCal;

  lcd.setCursor(0, 1);
  if      (rCalc < 1000)    { lcd.print(rCalc, 1);           lcd.print(" Ohm  "); }
  else if (rCalc < 1000000) { lcd.print(rCalc / 1000.0, 2);  lcd.print(" kOhm "); }
  else                      { lcd.print(rCalc / 1000000.0, 2); lcd.print(" MOhm"); }
}
```

</details>

---

## 👥 Team

| # | Name | Student ID |
|---|---|---|
| 1 | Mohamed Amr Ibrahim | 24010642 |
| 2 | Mohamed Abd El Aziz Mohamed | 24010628 |
| 3 | Mohamed Ali Mohamed | 24010638 |

**Supervisors:** Dr. Thanaa · Dr. Mai Banawan · Dr. Tarek Negm

📅 **Date:** Monday, 5 April 2026

---

## 📄 License

This project is open-source and available under the [MIT License](LICENSE).

---

<p align="center">Made with ❤️ using Arduino UNO</p>
