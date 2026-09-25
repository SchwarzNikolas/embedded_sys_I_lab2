#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"
#include "drivers/CF128x128x16_ST7735S.h"

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
    while(1);
}
#endif

#define FILTERSIZE 5

//*****************************************************************************
//
// Struct which stores previous values for average calculation
//
//*****************************************************************************
typedef struct AverageReadings {
    uint32_t buffer[FILTERSIZE];
    uint8_t index;
    uint32_t sum;
} AverageReadings;

//*****************************************************************************
//
// Struct to hold all the analog values
//
//*****************************************************************************
typedef struct AnalogValues {
    uint32_t JoystickX;
    uint32_t JoystickY;
    uint32_t Mic;
    uint32_t AccelerometerX;
    uint32_t AccelerometerY;
    uint32_t AccelerometerZ;
} AnalogValues;

//*****************************************************************************
//
// Init the average buffer by filling it with the first read value and
// calculating a preliminary sum
//
//*****************************************************************************
void initAverage(AverageReadings *average_readings, uint32_t inital_value) {
    uint32_t i;
    for(i = 0; i < FILTERSIZE; i++)
    {
        average_readings->buffer[i] = inital_value;
    }
    average_readings->index = 1;
    average_readings->sum = inital_value*FILTERSIZE;
}

//*****************************************************************************
//
// Update the average by replacing old value and calculating new sum
//
//*****************************************************************************
uint32_t updateAverage(AverageReadings *average_readings, uint32_t new_value){
    average_readings->sum -= average_readings->buffer[average_readings->index];
    average_readings->buffer[average_readings->index] = new_value;
    average_readings->index = (average_readings->index + 1) % FILTERSIZE;
    average_readings->sum += new_value;
    return average_readings->sum / FILTERSIZE;
}

//*****************************************************************************
//
// Init the ADC component.
//
// Step 0: Joystick Y
// Step 1: Accelerometer Z
// Step 2: Accelerometer Y
// Step 3: Accelerometer X
// Step 4: Mic
// Step 5: Joystick X
//
//*****************************************************************************
void ADCinit(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0) || !SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOE)) {}
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5);
    ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_CH0);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 1, ADC_CTL_CH1);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 2, ADC_CTL_CH2);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 3, ADC_CTL_CH3);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 4, ADC_CTL_CH8);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 5, ADC_CTL_CH9 | ADC_CTL_IE | ADC_CTL_END);
    ADCSequenceEnable(ADC0_BASE, 0);
    ADCIntClear(ADC0_BASE, 0);
}

//*****************************************************************************
//
// Read the analog values from the ADC.
//
//*****************************************************************************
void AdcRead(AnalogValues *analog_values)
{
    uint32_t samples[6];
    ADCProcessorTrigger(ADC0_BASE, 0);
    while(!ADCIntStatus(ADC0_BASE, 0, false)) {}
    ADCIntClear(ADC0_BASE, 0);
    ADCSequenceDataGet(ADC0_BASE, 0, samples);

    // Store the values in the struct
    analog_values->JoystickY = samples[0];
    analog_values->JoystickX = samples[5];
    analog_values->AccelerometerX = samples[3];
    analog_values->AccelerometerY = samples[2];
    analog_values->AccelerometerZ = samples[1];
    analog_values->Mic = samples[4];
}

// create struct for average readings of all components
AverageReadings avgJoyX, avgJoyY, avgMic, avgAccX, avgAccY, avgAccZ;

int main(void)
{
    // configure clock
    uint32_t sysClock = SysCtlClockFreqSet(
        (SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480),
        120000000);

    // init the needed components
    InitSysTickDelay(sysClock, 1000);
    CF128x128x16_ST7735SInit(sysClock);
    ADCinit();

    // read the first analog value and fill the buffer with read value
    AnalogValues analog;
    AdcRead(&raw);
    initAverage(&avgJoyX, raw.JoystickX);
    initAverage(&avgJoyY, raw.JoystickY);
    initAverage(&avgMic, raw.Mic);
    initAverage(&avgAccX, raw.AccelerometerX);
    initAverage(&avgAccY, raw.AccelerometerY);
    initAverage(&avgAccZ, raw.AccelerometerZ);

    // prepare graphical interface
    tContext sContext;
    GrContextInit(&sContext, &g_sCF128x128x16_ST7735S);
    GrContextFontSet(&sContext, &g_sFontCm14);
    tRectangle eraseArea = {0, 10, 127, 130};

    // loop wich reads the analog values and updates the display
    while(1)
    {
        // read the values, write it to buffer, so that the moving average applies
        AdcRead(&raw);
        AnalogValues filtered;
        filtered.JoystickX = updateAverage(&avgJoyX, raw.JoystickX);
        filtered.JoystickY = updateAverage(&avgJoyY, raw.JoystickY);
        filtered.Mic = updateAverage(&avgMic,  raw.Mic);
        filtered.AccelerometerX = updateAverage(&avgAccX, raw.AccelerometerX);
        filtered.AccelerometerY = updateAverage(&avgAccY, raw.AccelerometerY);
        filtered.AccelerometerZ = updateAverage(&avgAccZ, raw.AccelerometerZ);

        // erase screen
        GrContextForegroundSet(&sContext, ClrBlack);
        GrRectFill(&sContext, &eraseArea);
        GrContextForegroundSet(&sContext, ClrWhite);

        // write new average to screen
        char strBuf[20];
        snprintf(strBuf, sizeof(strBuf), "Joystick X:%4u", filtered.JoystickX);
        GrStringDraw(&sContext, strBuf, -1, 2, 10, false);
        snprintf(strBuf, sizeof(strBuf), "Joystick Y:%4u", filtered.JoystickY);
        GrStringDraw(&sContext, strBuf, -1, 2, 24, false);
        snprintf(strBuf, sizeof(strBuf), "Mic:%4u", filtered.Mic);
        GrStringDraw(&sContext, strBuf, -1, 2, 38, false);
        snprintf(strBuf, sizeof(strBuf), "Accelerator X:%4u", filtered.AccelerometerX);
        GrStringDraw(&sContext, strBuf, -1, 2, 52, false);
        snprintf(strBuf, sizeof(strBuf), "Accelerator Y:%4u", filtered.AccelerometerY);
        GrStringDraw(&sContext, strBuf, -1, 2, 66, false);
        snprintf(strBuf, sizeof(strBuf), "Accelerator Z:%4u", filtered.AccelerometerZ);
        GrStringDraw(&sContext, strBuf, -1, 2, 80, false);

        SysTickDelayMs(100);
    }
}
