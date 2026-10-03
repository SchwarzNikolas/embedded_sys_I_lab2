#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>

#define CELL_SIZE 5

#define DISPLAY_MAX_SIZE 127

void init_display(uint32_t sysClock);
void draw_game();

#endif
