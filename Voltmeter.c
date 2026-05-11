#include "Voltmeter.h"
#include "Constants.h"

float voltCal = 1.036; [cite: 308]

float getVoltage() {
    analogReference(DEFAULT); [cite: 324]
    delay(15); 
    
    float adc = readADC(PIN_VOLT, 30); [cite: 326]
    float vOut = (adc * VCC_REF) / ADC_MAX; [cite: 327]
    float finalV = vOut * ((R_DIV_TOP + R_DIV_BOTTOM) / R_DIV_BOTTOM) * voltCal; [cite: 327]
    
    return (finalV < 0.05) ? 0.0 : finalV; [cite: 328]
}
