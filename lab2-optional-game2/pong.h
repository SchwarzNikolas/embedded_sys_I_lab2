#ifndef PONG_H
#define PONG_H

#include <stdint.h>
#include <stdbool.h>
#include "adc.h"

#define BOARD_WIDTH 51
#define BOARD_HEIGHT 51
#define PADDLE_HEIGHT 5
#define PADDLE_WIDTH 2

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
        uint8_t speed;
} Ball;

typedef struct {
	Position position;
        uint8_t speed;
} Paddle;


extern Ball ball;
extern Paddle player;
extern Paddle computer;
extern uint16_t player_score;
extern uint16_t computer_score;
extern bool running;

void init_game(void);
void handle_joystick_input(AnalogValues *analog_values);
void update_game(void);

#endif
