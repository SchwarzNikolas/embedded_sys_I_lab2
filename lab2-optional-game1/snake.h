#ifndef SNAKE_H
#define SNAKE_H

#include <stdint.h>
#include <stdbool.h>
#include "adc.h"

#define MAX_SNAKE_LENGTH 100
#define BOARD_WIDTH 11
#define BOARD_HEIGHT 11

typedef struct Position {
	uint8_t x;
	uint8_t y;
} Position;

typedef enum Direction {
	UP,
	DOWN,
	LEFT,
	RIGHT
} Direction;

typedef struct Snake {
	Position body[MAX_SNAKE_LENGTH];
	uint8_t length;
	Direction direction;
} Snake;

typedef struct Apple {
	Position position;
} Apple;


extern Snake snake;
extern Apple apple;
extern uint8_t score;
extern bool running;

void init_game(void);
void handle_joystick_input(AnalogValues *analog_values);
void update_game(void);

#endif
