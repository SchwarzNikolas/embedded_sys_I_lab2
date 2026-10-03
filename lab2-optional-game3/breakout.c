#include "breakout.h"
#include <stdlib.h>

Ball ball;
Paddle player;
uint16_t score;
Brick bricks[BRICK_ROWS][BRICK_COLUMNS];
bool running;
bool won;
uint16_t paddle_bonus = 100;

//*****************************************************************************
//
// Parse the raw joystick values into directions.
//
//*****************************************************************************
void handle_joystick_input(AnalogValues *analog_values) {
        if (analog_values->JoystickX > 3500) {
                player.speed = 3;
                if (player.position.x < BOARD_WIDTH - (PADDLE_WIDTH / 2) - 2) {
                        player.position.x += player.speed;
                }
        }
        else if (analog_values->JoystickX > 3000) {
                player.speed = 2;
                if (player.position.x < BOARD_WIDTH - (PADDLE_WIDTH / 2) - 1) {
                        player.position.x += player.speed;
                }
        }
        else if (analog_values->JoystickX > 2500) {
                player.speed = 1;
                if (player.position.x < BOARD_WIDTH - (PADDLE_WIDTH / 2)) {
                        player.position.x += player.speed;
                }
        }
        else if (analog_values->JoystickX < 500) {
                player.speed = 3;
                if (player.position.x > (PADDLE_WIDTH / 2) + 2) {
                        player.position.x -= player.speed;
                }
        }
        else if (analog_values->JoystickX < 1000) {
                player.speed = 2;
                if (player.position.x > (PADDLE_WIDTH / 2) + 1) {
                        player.position.x -= player.speed;
                }
        }
        else if (analog_values->JoystickX < 1500) {
                player.speed = 1;
                if (player.position.x > (PADDLE_WIDTH / 2)) {
                        player.position.x -= player.speed;
                }
        }
        else {
                player.speed = 0;
        }
}

//*****************************************************************************
//
// Move the ball according to its velocity.
//
//*****************************************************************************
void move_ball(void) {
        ball.position.x += ball.velocity.x;
        ball.position.y += ball.velocity.y;
}

//*****************************************************************************
//
// Checks if the ball is in the wall
//
//*****************************************************************************
void check_wall_collision(void) {
        if (ball.position.y <= 0) {
                ball.position.y = 0;
                ball.velocity.y = -ball.velocity.y;
        } else if (ball.position.y >= BOARD_HEIGHT - 1) {
                running = false;
        }
        if (ball.position.x <= 0) {
                ball.position.x = 0;
                ball.velocity.x = -ball.velocity.x;
        } else if (ball.position.x >= BOARD_WIDTH - 1) {
                ball.position.x = BOARD_WIDTH - 1;
                ball.velocity.x = -ball.velocity.x;
        }

}

//*****************************************************************************
//
// Checks if the ball is hitting a paddle
//
//*****************************************************************************
void check_paddle_collision(void) {
        // Player paddle
        if (ball.velocity.y > 0 && ball.position.y >= player.position.y && 
        ball.position.x >= player.position.x - (PADDLE_WIDTH / 2) && 
        ball.position.x <= player.position.x + (PADDLE_WIDTH / 2)) {
                ball.position.y = player.position.y - 1;
                ball.velocity.y = -ball.velocity.y;
                if (paddle_bonus > 1) { 
                        paddle_bonus = paddle_bonus / 2;
                }
        }
}

//*****************************************************************************
//
// Reset the ball to default values.
//
//*****************************************************************************
void reset_ball(void) {
        ball.position.x = BOARD_WIDTH / 2;
        ball.position.y = BOARD_HEIGHT / 2;

        ball.velocity.x = 1;
        ball.velocity.y = -1;
}

//*****************************************************************************
//
// Setup the game.
//
//*****************************************************************************
void init_game() {
        score = 0;
        player.position.x = BOARD_WIDTH / 2;
        player.position.y = BOARD_HEIGHT - PADDLE_HEIGHT;
        player.speed = 0;

        uint8_t row;
        uint8_t column;
        for (row = 0; row < BRICK_ROWS; row++) {
                for (column = 0; column < BRICK_COLUMNS; column++) {
                        bricks[row][column].position.x = column * (BRICK_WIDTH + 1) + 2;
                        bricks[row][column].position.y = row * (BRICK_HEIGHT + 1) + 2;
                        bricks[row][column].destroyed = false;
                }
        }

        reset_ball();
	running = true;
        won = false;
}

//*****************************************************************************
//
// Check if ball hit brick.
//
//*****************************************************************************
bool check_hit_brick(void) {
        uint8_t row;
        uint8_t column;
        bool bricks_exist = false;

        for (row = 0; row < BRICK_ROWS; row++) {
                for (column = 0; column < BRICK_COLUMNS; column++) {
                        if (!bricks[row][column].destroyed) {
                                if (ball.position.x >= bricks[row][column].position.x &&
                                ball.position.x < bricks[row][column].position.x + BRICK_WIDTH &&
                                ball.position.y >= bricks[row][column].position.y &&
                                ball.position.y < bricks[row][column].position.y + BRICK_HEIGHT) {
                                        bricks[row][column].destroyed = true;
                                        score = score + paddle_bonus;
                                        ball.velocity.y = -ball.velocity.y;
                                        continue;
                                }
                                bricks_exist = true;
                        }
                }
        }

        return bricks_exist;
}

//*****************************************************************************
//
// Move the ball and paddle and check if game is done.
//
//*****************************************************************************
void update_game(void) {
        move_ball();

        check_wall_collision();

        check_paddle_collision();

        if (!check_hit_brick()){
                won = true;
                running = false;
        };
}
