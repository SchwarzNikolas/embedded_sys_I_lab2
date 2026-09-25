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
// Send a string to the UART.
//
//*****************************************************************************
void
UARTSend(const uint8_t *pui8Buffer, uint32_t ui32Count)
{
    //
    // Loop while there are more characters to send.
    //
    while(ui32Count--)
    {
        //
        // Write the next character to the UART.
        //
        MAP_UARTCharPutNonBlocking(UART0_BASE, *pui8Buffer++);
    }
}

//*****************************************************************************
//
// The UART interrupt handler.
//
//*****************************************************************************
void
UARTIntHandler(void)
{
    uint32_t ui32Status;

    //
    // Get the interrrupt status.
    //
    ui32Status = MAP_UARTIntStatus(UART0_BASE, true);

    //
    // Clear the asserted interrupts.
    //
    MAP_UARTIntClear(UART0_BASE, ui32Status);


    // array to hold the number received from UART
    char number[5] = "";
    uint8_t index = 0;


    //
    // Loop while there are characters in the receive FIFO.
    //
    while(MAP_UARTCharsAvail(UART0_BASE))
    {
        //
        // Read the next character from the UART and write it back to the UART.
        //
        char c = MAP_UARTCharGetNonBlocking(UART0_BASE);

        // append the character to the number array if it is not a newline or carriage return
        if(index < sizeof(number) - 1)
        {
            number[index++] = c;
            number[index] = '\0';
        }

        //
        // Blink the LED to show a character transfer is occuring.
        //
        MAP_GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, GPIO_PIN_0);

        //
        // Delay for 1 millisecond.  Each SysCtlDelay is about 3 clocks.
        //
        SysCtlDelay(g_ui32SysClock / (1000 * 3));

        //
        // Turn off the LED
        //
        MAP_GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, 0);
    }

    // convert the number to an integer and set the pwm_value if the number is not empty
    if (strlen(number) > 0) {
        pwm_value = atoi(number);
        UARTSend((uint8_t *)"Enter a number: \n", 18);
    }
    strcpy(number, "");
    
}

//*****************************************************************************
//
// Init the UART component.
//
//*****************************************************************************
void UARTinit(){
        //
    // Enable the GPIO port that is used for the on-board LED.
    //
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);

    //
    // Enable the GPIO pins for the LED (PN0).
    //
    MAP_GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0);

    //
    // Enable the peripherals used by this example.
    //
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //
    // Enable processor interrupts.
    //
    MAP_IntMasterEnable();

    //
    // Set GPIO A0 and A1 as UART pins.
    //
    MAP_GPIOPinConfigure(GPIO_PA0_U0RX);
    MAP_GPIOPinConfigure(GPIO_PA1_U0TX);
    MAP_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //
    // Configure the UART for 115,200, 8-N-1 operation.
    //
    MAP_UARTConfigSetExpClk(UART0_BASE, g_ui32SysClock, 115200,
                            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                             UART_CONFIG_PAR_NONE));

    //
    // Enable the UART interrupt.
    //
    MAP_IntEnable(INT_UART0);
    MAP_UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);
}

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
// Read number from UART and set PWM according to read value..
//
//*****************************************************************************
int
main(void)
{
    // init the system clock to 16 kHz
    g_ui32SysClock = MAP_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                             SYSCTL_OSC_MAIN |
                                             SYSCTL_USE_PLL |
                                             SYSCTL_CFG_VCO_480), 16000);
                                             
    // Initialize the UART and send first message
    UARTinit();
    UARTSend((uint8_t *)"Enter a number: \n", 18);

    // Calculate PWM clock and word and init PWM
    uint32_t pwmClock = g_ui32SysClock / 16;
    uint32_t pwm_word = pwmClock / 10;
    PWMinit();

    while(1)
    {
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
            // Define the GPIO pin as PWM output and set the PWM parameters according to the UART input
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
