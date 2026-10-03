#include <stdio.h>
#include "breakout.h"
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
void draw_game(void) {
        char scoreText[20];
        if (running) {
                tRectangle rect;
                uint8_t row;
                uint8_t column;

                clear_display();

                // calculate where the board starts on the display.
                int16_t boardX = (DISPLAY_MAX_SIZE - BOARD_WIDTH * CELL_SIZE) / 2;

                int16_t boardY = (DISPLAY_MAX_SIZE - BOARD_HEIGHT * CELL_SIZE) / 2 + 5;

                // border
                rect.i16XMin = boardX - 1;
                rect.i16YMin = boardY - 1;
                rect.i16XMax = boardX + BOARD_WIDTH * CELL_SIZE;
                rect.i16YMax = boardY + BOARD_HEIGHT * CELL_SIZE;

                GrContextForegroundSet(&sContext, ClrWhite);
                GrRectDraw(&sContext, &rect);

                // bricks that are not destroyed
                for (row = 0; row < BRICK_ROWS; row++) {
                        for (column = 0; column < BRICK_COLUMNS; column++) {
                                if (!bricks[row][column].destroyed) {
                                        rect.i16XMin = boardX + bricks[row][column].position.x * CELL_SIZE;
                                        rect.i16YMin = boardY + bricks[row][column].position.y * CELL_SIZE;
                                        rect.i16XMax = rect.i16XMin + BRICK_WIDTH * CELL_SIZE - 1;
                                        rect.i16YMax = rect.i16YMin + BRICK_HEIGHT * CELL_SIZE - 1;
                                        GrContextForegroundSet(&sContext, ClrOrange);
                                        GrRectFill(&sContext, &rect);
                                }
                        }
                }

                // player paddle
                rect.i16XMin = boardX + (player.position.x - PADDLE_WIDTH / 2) * CELL_SIZE;
                rect.i16YMin = boardY + player.position.y * CELL_SIZE;
                rect.i16XMax = rect.i16XMin + PADDLE_WIDTH * CELL_SIZE - 1;
                rect.i16YMax = rect.i16YMin + PADDLE_HEIGHT * CELL_SIZE - 1;
                GrContextForegroundSet(&sContext, ClrWhite);
                GrRectFill(&sContext, &rect);

                // ball
                rect.i16XMin = boardX + ball.position.x * CELL_SIZE;
                rect.i16YMin = boardY + ball.position.y * CELL_SIZE;
                rect.i16XMax = rect.i16XMin + CELL_SIZE - 1;
                rect.i16YMax = rect.i16YMin + CELL_SIZE - 1;
                GrContextForegroundSet(&sContext, ClrBlue);
                GrRectFill(&sContext, &rect);

                // score
                sprintf(scoreText, "Score: %u", (unsigned int) score);
                GrContextForegroundSet(&sContext, ClrWhite);
                GrStringDraw(&sContext, scoreText, -1, 5, 2, true);
        } else {
                if (won) {
                        clear_display();
                        GrContextForegroundSet(&sContext, ClrGreen);
                        GrStringDraw(&sContext, "YOU WIN!", -1, 25, 59, true);
                } else {
                        clear_display();
                        GrContextForegroundSet(&sContext, ClrRed);
                        GrStringDraw(&sContext, "GAME-OVER", -1, 23, 59, true);
                }
                sprintf(scoreText, "Score: %u", (unsigned int) score);
                GrContextForegroundSet(&sContext, ClrWhite);
                GrStringDraw(&sContext, scoreText, -1, 25, 79, true);
        }
}
