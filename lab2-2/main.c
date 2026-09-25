#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"
#include "driverlib/uart.h"
#include "driverlib/pwm.h"

volatile uint8_t pwm_value = 0;
uint32_t g_ui32SysClock;

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif

//*****************************************************************************
//
// Init the PWM component.
//
//*****************************************************************************
void PWMinit(){
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPWMClockSet(SYSCTL_PWMDIV_16);
    SysCtlPeripheralDisable(SYSCTL_PERIPH_PWM0);
    SysCtlPeripheralReset(SYSCTL_PERIPH_PWM0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);
    GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_2);
    GPIOPinConfigure(GPIO_PF2_M0PWM2);              // Ensure that the pin is counted as only pwm (?)
    PWMGenConfigure(PWM0_BASE, PWM_GEN_1, PWM_GEN_MODE_DOWN |
    PWM_GEN_MODE_NO_SYNC | PWM_GEN_MODE_DBG_RUN);
}

//*****************************************************************************
//
// Init the ADC component.
//
//*****************************************************************************
void ADCinit(){
    // Activate the peripherals
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

    // Ensure that the peripheral is ready for use
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0) || !SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOE)) {}
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_4);
    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH9 | ADC_CTL_END | ADC_CTL_IE);
    ADCSequenceEnable(ADC0_BASE, 3);
    ADCIntClear(ADC0_BASE, 3);
}

int
main(void)
{
    // init the system clock to 16 kHz
    g_ui32SysClock = MAP_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                             SYSCTL_OSC_MAIN |
                                             SYSCTL_USE_PLL |
                                             SYSCTL_CFG_VCO_480), 16000);
                                             
    // Calculate PWM clock and word and init PWM
    uint32_t pwmClock = g_ui32SysClock / 16;
    uint32_t pwm_word = pwmClock / 10;
    PWMinit();

    // Initialise the ADC module
    ADCinit();
    uint32_t analog_value;

    while(1)
    {
        // start sampling an analog value
        ADCProcessorTrigger(ADC0_BASE, 3);

        // wait until the value is read completely and write to the analog variable
        while(!ADCIntStatus(ADC0_BASE, 3, false)){}
        ADCIntClear(ADC0_BASE, 3);
        ADCSequenceDataGet(ADC0_BASE, 3, &analog_value);
        
        // Calculate the percentage value based on the maximum (4095) from the analogue stick
        pwm_value = (analog_value * 100) / 4095;

        if (pwm_value == 100){
            // Define the GPIO pin as digital output again and set it to high
            GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2);
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2);
        }
        else if (pwm_value == 0){
            // Define the GPIO pin as digital output again and set it to low
            GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2);
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0);
        }
        else{
            // Define the GPIO pin as PWM output and set the PWM parameters according to the joystick input
            GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_2);
            GPIOPinConfigure(GPIO_PF2_M0PWM2);              // Set GPIO to PWM inputs only
            PWMGenPeriodSet(PWM0_BASE, PWM_GEN_1, pwm_word);
            PWMPulseWidthSet(PWM0_BASE, PWM_OUT_2, (pwm_word * pwm_value) / 100);
            PWMGenEnable(PWM0_BASE, PWM_GEN_1);
            PWMOutputState(PWM0_BASE, PWM_OUT_2_BIT, true);
        };
       
        // Delay for 1 ms
        SysCtlDelay(g_ui32SysClock / (1000 *3));
    }
}
