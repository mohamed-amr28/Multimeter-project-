#include "Ohmmeter.h"
#include "Constants.h"
#include <LiquidCrystal_I2C.h>

extern LiquidCrystal_I2C lcd;
extern int currentRange;
float resCal = 1.025; [cite: 308]

void showResistance() {
    analogReference(DEFAULT); [cite: 348]
    delay(15);
    
    float activeRef;
    if (currentRange == 0) { // LOW
        pinMode(PIN_REF_MID, INPUT); pinMode(PIN_REF_HIGH, INPUT);
        pinMode(PIN_REF_LOW, OUTPUT); digitalWrite(PIN_REF_LOW, HIGH); [cite: 349]
        activeRef = R_REF_LOW;
        lcd.setCursor(0, 0); lcd.print("Res [Low 1k]:   "); [cite: 350]
    } 
    // ... Repeat for MID and HIGH ranges as per original logic [cite: 351, 352, 353]

    delay(10);
    float adc = readADC(PIN_RES, 30); [cite: 353]
    if (adc >= 1015) { lcd.setCursor(0, 1); lcd.print("OL (Open)        "); return; } [cite: 354]
    
    float vOut = (adc * VCC_REF) / ADC_MAX; [cite: 355]
    float rCalc = activeRef * (vOut / (VCC_REF - vOut)) * resCal; [cite: 356]

    lcd.setCursor(0, 1);
    if (rCalc < 1000) { lcd.print(rCalc, 1); lcd.print(" Ohm      "); } [cite: 358]
    else if (rCalc < 1000000) { lcd.print(rCalc / 1000.0, 2); lcd.print(" kOhm     "); } [cite: 359]
    else { lcd.print(rCalc / 1000000.0, 2); lcd.print(" MOhm     "); } [cite: 360]
}
