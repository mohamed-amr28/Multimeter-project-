#include "Ammeter.h"
#include "Constants.h"

float currCal = 1.023; [cite: 308]
float filteredCurrent = 0.0; [cite: 309]

float getCurrent() {
    analogReference(INTERNAL); [cite: 328]
    delay(15); 
    
    float adc_avg = readADC(PIN_CURR, 200); [cite: 329]
    float vSense = (adc_avg * INTERNAL_REF) / ADC_MAX; [cite: 330]
    float rawCurrent = (vSense / (R_SHUNT * AMP_GAIN)) * currCal; [cite: 330]

    float correctedCurrent = (rawCurrent < 0.090) ? (rawCurrent * 1.16) : rawCurrent; [cite: 332, 333]

    float diff = abs(correctedCurrent - filteredCurrent); [cite: 334]
    if (diff > 0.030) { 
        filteredCurrent = correctedCurrent; [cite: 335]
    } else {
        float alpha = (rawCurrent < 0.05) ? 0.15 : 0.3; [cite: 336, 337]
        filteredCurrent = (alpha * correctedCurrent) + (1.0 - alpha) * filteredCurrent; [cite: 337]
    }
    return (filteredCurrent < 0.001) ? 0.0 : filteredCurrent; [cite: 338]
}v
