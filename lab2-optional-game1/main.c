#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "adc.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "drivers/CF128x128x16_ST7735S.h"
#include "inc/hw_memmap.h"
#include "snake.h"

#define CELL_SIZE 8
#define BOARD_Y 9
#define DISPLAY_MAX_SIZE 127

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
// Displays everything that should be on the display.
//
//*****************************************************************************
void draw_game(tContext *sContext) {
	tRectangle rect;
	uint8_t i;

	if (running) {
		// draw border
		rect.i16XMin = DISPLAY_MAX_SIZE / 2 - (BOARD_WIDTH * CELL_SIZE) / 2 - 1;
		rect.i16YMin = BOARD_Y - 1;
		rect.i16XMax = DISPLAY_MAX_SIZE / 2 + (BOARD_WIDTH * CELL_SIZE) / 2;
		rect.i16YMax = BOARD_Y + BOARD_WIDTH * CELL_SIZE;

		GrContextForegroundSet(sContext, ClrWhite);
		GrRectDraw(sContext, &rect);

		// draw snake
		for (i = 0; i < snake.length; i++) {
			rect.i16XMin = DISPLAY_MAX_SIZE / 2 - (BOARD_WIDTH * CELL_SIZE) / 2 + snake.body[i].x * CELL_SIZE;
			rect.i16YMin = BOARD_Y + snake.body[i].y * CELL_SIZE;
			rect.i16XMax = rect.i16XMin + CELL_SIZE - 1;
			rect.i16YMax = rect.i16YMin + CELL_SIZE - 1;

			if (i == 0) {
				GrContextForegroundSet(sContext, ClrWhite);
			} else {
				GrContextForegroundSet(sContext, ClrGreen);
			}
			GrRectFill(sContext, &rect);
		}
		
		// draw apple
		rect.i16XMin = DISPLAY_MAX_SIZE / 2 - (BOARD_WIDTH * CELL_SIZE) / 2 + apple.position.x * CELL_SIZE;
		rect.i16YMin = BOARD_Y + apple.position.y * CELL_SIZE;
		rect.i16XMax = rect.i16XMin + CELL_SIZE - 1;
		rect.i16YMax = rect.i16YMin + CELL_SIZE - 1;
		GrContextForegroundSet(sContext, ClrRed);
		GrRectFill(sContext, &rect);

		// draw score
		char scoreText[20];
		sprintf(scoreText, "Score: %d", score);
		GrContextForegroundSet(sContext, ClrWhite);
		GrStringDraw(sContext, scoreText, -1, 2, 110, true);
	} else {
		// draw end of game
		char gameOver[10] = "GAME-OVER";
		GrContextForegroundSet(sContext, ClrRed);
		GrStringDraw(sContext, gameOver, -1, 23, 59, true);
	}
}

//*****************************************************************************
//
// Start the game, read analog inputs and update the game state.
//
//*****************************************************************************
int main(void) {
	uint32_t sysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 120000000);

	InitSysTickDelay(sysClock, 1000);
	CF128x128x16_ST7735SInit(sysClock);
	ADCinit();

	AnalogValues analog;

	tContext sContext;
	GrContextInit(&sContext, &g_sCF128x128x16_ST7735S);
	GrContextFontSet(&sContext, &g_sFontCm14);
	tRectangle eraseArea = {0, 0, 127, 127};

	// initialize snake
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

			// clear display
			GrContextForegroundSet(&sContext, ClrBlack);
			GrRectFill(&sContext, &eraseArea);
			GrContextForegroundSet(&sContext, ClrWhite);

			// draw the game
			draw_game(&sContext);
			moveCounter = 0;
		}

		SysTickDelayMs(10);
	}
}
