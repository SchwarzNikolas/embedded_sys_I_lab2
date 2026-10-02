#include <stdbool.h>
#include "adc.h"
#include "inc/hw_memmap.h"
#include "driverlib/adc.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

//*****************************************************************************
//
// Init the ADC component.
//
// Step 0: Joystick Y
// Step 1: Joystick X
//
//*****************************************************************************
void ADCinit(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0) || !SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOE)) {}
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5);
    ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceStepConfigure(ADC0_BASE, 1, 0, ADC_CTL_CH0);
    ADCSequenceStepConfigure(ADC0_BASE, 1, 1, ADC_CTL_CH9 | ADC_CTL_IE | ADC_CTL_END);
    ADCSequenceEnable(ADC0_BASE, 1);
    ADCIntClear(ADC0_BASE, 1);
}

//*****************************************************************************
//
// Read the analog values from the ADC.
//
//*****************************************************************************
void AdcRead(AnalogValues *analog_values)
{
    uint32_t samples[2];
    ADCProcessorTrigger(ADC0_BASE, 1);
    while(!ADCIntStatus(ADC0_BASE, 1, false)) {}
    ADCIntClear(ADC0_BASE, 1);
    ADCSequenceDataGet(ADC0_BASE, 1, samples);

    // Store the values in the struct
    analog_values->JoystickY = samples[0];
    analog_values->JoystickX = samples[1];
}
