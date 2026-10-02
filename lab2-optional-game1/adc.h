#ifndef ADC_H
#define ADC_H

#include <stdint.h>

//*****************************************************************************
//
// Struct to hold all the analog values
//
//*****************************************************************************
typedef struct AnalogValues {
    uint32_t JoystickX;
    uint32_t JoystickY;
} AnalogValues;

void ADCinit(void);
void AdcRead(AnalogValues *analog_values);

#endif
