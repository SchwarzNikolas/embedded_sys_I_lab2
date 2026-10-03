#ifndef PONG_H
#define PONG_H

#include <stdint.h>
#include <stdbool.h>
#include "adc.h"

#define BOARD_WIDTH 51
#define BOARD_HEIGHT 51
#define PADDLE_HEIGHT 2
#define PADDLE_WIDTH 7
#define BRICK_WIDTH 6
#define BRICK_HEIGHT 2
#define BRICK_COLUMNS 7
#define BRICK_ROWS 5


typedef struct {
	int8_t x;
	int8_t y;
} Position;

typedef struct {
	int8_t x;
	int8_t y;
} Velocity;

typedef struct {
	Position position;
        Velocity velocity;
} Ball;

typedef struct {
	Position position;
        uint8_t speed;
} Paddle;

typedef struct {
    Position position;
    bool destroyed;
} Brick;


extern Ball ball;
extern Paddle player;
extern uint16_t score;
extern Brick bricks[BRICK_ROWS][BRICK_COLUMNS];
extern bool running;
extern bool won;

void init_game(void);
void handle_joystick_input(AnalogValues *analog_values);
void update_game(void);

#endif
