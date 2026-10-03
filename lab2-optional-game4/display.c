#include <stdio.h>
#include "maze.h"
#include "display.h"
#include "drivers/CF128x128x16_ST7735S.h"

tContext sContext;

void clear_display() {
	tRectangle eraseArea = {0, 0, 127, 127};
        GrContextForegroundSet(&sContext, ClrBlack);
	GrRectFill(&sContext, &eraseArea);
}

void init_display(uint32_t sysClock) {
        CF128x128x16_ST7735SInit(sysClock);
        GrContextInit(&sContext, &g_sCF128x128x16_ST7735S);
	GrContextFontSet(&sContext, &g_sFontCm14);
        clear_display();
}

//*****************************************************************************
//
// Displays everything that should be on the display.
//
//*****************************************************************************
void draw_game(void)
{
        tRectangle rect;
        uint8_t row;
        uint8_t column;

        clear_display();

        // calc board coords
        int16_t boardX = (DISPLAY_MAX_SIZE - BOARD_COLUMNS * CELL_SIZE) / 2;

        int16_t boardY = (DISPLAY_MAX_SIZE - BOARD_ROWS * CELL_SIZE) / 2;

        // maze
        for (row = 0; row < BOARD_ROWS; row++) {
                for (column = 0; column < BOARD_COLUMNS; column++) {
                rect.i16XMin = boardX + column * CELL_SIZE;
                rect.i16YMin = boardY + row * CELL_SIZE;
                rect.i16XMax = rect.i16XMin + CELL_SIZE - 1;
                rect.i16YMax = rect.i16YMin + CELL_SIZE - 1;

                if (maze[row][column] == WALL) {
                        GrContextForegroundSet(&sContext, ClrWhite);
                        GrRectFill(&sContext, &rect);

                } else if (maze[row][column] == GOAL) {
                        GrContextForegroundSet(&sContext, ClrRed);
                        GrRectFill(&sContext, &rect);
                } else if (maze[row][column] == VISITED) {
                        GrContextForegroundSet(&sContext, ClrDarkGray);
                        GrRectFill(&sContext, &rect);
                } else {
                        GrContextForegroundSet(&sContext, ClrWhite);
                        GrRectFill(&sContext, &rect);
                }
                }
        }

        //player
        rect.i16XMin = boardX + player.position.x * CELL_SIZE;
        rect.i16YMin = boardY + player.position.y * CELL_SIZE;
        rect.i16XMax = rect.i16XMin + CELL_SIZE - 1;
        rect.i16YMax = rect.i16YMin + CELL_SIZE - 1;
        GrContextForegroundSet(&sContext, ClrBlue);
        GrRectFill(&sContext, &rect);

        // win or death text
        if (!running) {
                if (won) {
                GrContextForegroundSet(&sContext, ClrGreen);
                GrStringDraw(&sContext, "YOU WIN!", -1, 35, 60, true);
                } else {
                GrContextForegroundSet(&sContext, ClrRed);
                GrStringDraw(&sContext, "GAME OVER", -1, 25, 60, true);
                }
        }
}
