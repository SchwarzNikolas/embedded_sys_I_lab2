#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "adc.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "inc/hw_memmap.h"
#include "drivers/CF128x128x16_ST7735S.h"
#include "maze.h"
#include "display.h"

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void __error__(char *pcFilename, uint32_t ui32Line) {
	while (1);
}
#endif

//*****************************************************************************
//
// Start the game, read analog inputs and update the game state.
//
//*****************************************************************************
int main(void) {
	uint32_t sysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 120000000);

	InitSysTickDelay(sysClock, 1000);
	init_display(sysClock);
	ADCinit();

	AnalogValues analog;

	// initialize maze
	init_game();
	uint8_t moveCounter = 0;
	while (1) {
		// read joystick
		AdcRead(&analog);
		handle_joystick_input(&analog);

		moveCounter++;

		if (moveCounter > 30) {
			if (running) {
				update_game();
			}

			// draw the game
			draw_game();
			moveCounter = 0;
		}

		SysTickDelayMs(10);
                if (!running) {
                        for(;;){}
                }
	}
}
