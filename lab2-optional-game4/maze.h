#ifndef MAZE_H
#define MAZE_H

#include <stdint.h>
#include <stdbool.h>
#include "adc.h"

#define BOARD_ROWS 21
#define BOARD_COLUMNS 21

typedef struct Position {
	uint8_t x;
	uint8_t y;
} Position;

typedef enum {
	UP,
	DOWN,
	LEFT,
	RIGHT,
        NONE,
} Direction;

typedef enum {
	EMPTY,
        WALL,
        VISITED,
        GOAL,
        START,
} MazeField;

typedef struct {
	Position position;
	Direction direction;
} Player;

typedef struct {
	Position position;
} Goal;


extern Player player;
extern Goal goal;
extern MazeField maze[BOARD_ROWS][BOARD_COLUMNS];
extern bool won;
extern bool running;

void init_game(void);
void handle_joystick_input(AnalogValues *analog_values);
void update_game(void);

#endif
