#include "snake.h"
#include <stdlib.h>

Snake snake;
Apple apple;
uint8_t score;
bool running;

//*****************************************************************************
//
// Generate an apple that is withing the game limit and not on the snake.
//
//*****************************************************************************
void generate_apple() {
	Position position;
	bool in_snake;
	do {
		position.x = rand() % BOARD_WIDTH;
		position.y = rand() % BOARD_HEIGHT;
		in_snake = false;
		uint8_t i;
		for (i = 0; i < snake.length; i++) {
			if (snake.body[i].x == position.x && snake.body[i].y == position.y) {
	in_snake = true;
	break;
			}
		}

	} while (in_snake);

	apple.position = position;
}

//*****************************************************************************
//
// Setup the game.
//
//*****************************************************************************
void init_game() {
	snake.length = 2;
	snake.direction = UP;
	snake.body[0] = (Position){BOARD_WIDTH / 2, BOARD_HEIGHT / 2};
	snake.body[1] = (Position){BOARD_WIDTH / 2, BOARD_HEIGHT / 2 + 1};
	score = 0;

	generate_apple();
	running = true;
}

//*****************************************************************************
//
// Parse the raw joystick values into directions.
//
//*****************************************************************************
void handle_joystick_input(AnalogValues *analog_values) {
	if (analog_values->JoystickY > 3000) {
		if (snake.direction != DOWN) {
			snake.direction = UP;
		}
	} else if (analog_values->JoystickY < 1000) {
		if (snake.direction != UP) {
			snake.direction = DOWN;
		}
	} else if (analog_values->JoystickX < 1000) {
		if (snake.direction != RIGHT) {
			snake.direction = LEFT;
		}
	} else if (analog_values->JoystickX > 3000) {
		if (snake.direction != LEFT) {
			snake.direction = RIGHT;
		}
	}
}

//*****************************************************************************
//
// Handle the movement of the snake and check if it ate.
//
//*****************************************************************************
void move_snake(void) {
	Position new_head;
	bool eating_apple = false;
	uint8_t i;

	// calculate position of the head
	new_head = snake.body[0];
	switch (snake.direction) {
	case UP:
		new_head.y--;
		break;

	case DOWN:
		new_head.y++;
		break;

	case LEFT:
		new_head.x--;
		break;

	case RIGHT:
		new_head.x++;
		break;
	}

	// snake eating an apple?!
	if (new_head.x == apple.position.x && new_head.y == apple.position.y) {
		eating_apple = true;
	}
	if (eating_apple && snake.length < MAX_SNAKE_LENGTH) {
		snake.length++;
		score++;
	}

	// move the snake
	for (i = snake.length - 1; i > 0; i--) {
		snake.body[i] = snake.body[i - 1];
	}

	// move the head
	snake.body[0] = new_head;

	if (eating_apple) {
		generate_apple();
	}
}

//*****************************************************************************
//
// Checks if snake is in the wall or eating itself.
//
//*****************************************************************************
bool check_collision() {
	Position position = snake.body[0];

	// collision with border
	if (position.x >= BOARD_WIDTH || position.y >= BOARD_HEIGHT) {
		return true;
	}

	// collision with itself
	uint8_t i;
	for (i = 1; i < snake.length; i++) {
		if (position.x == snake.body[i].x && position.y == snake.body[i].y) {
			return true;
		}
	}

	return false;
}

//*****************************************************************************
//
// Move the snake and check if game is done.
//
//*****************************************************************************
void update_game() {
	move_snake();

	if (check_collision()) {
		running = false;
		return;
	}
}
