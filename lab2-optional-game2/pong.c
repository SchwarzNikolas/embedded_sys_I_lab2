#include "pong.h"
#include <stdlib.h>

Ball ball;
Paddle player;
Paddle computer;
uint16_t player_score;
uint16_t computer_score;
bool running;

//*****************************************************************************
//
// Parse the raw joystick values into directions.
//
//*****************************************************************************
void handle_joystick_input(AnalogValues *analog_values) {
        if (analog_values->JoystickY > 3500) {
                player.speed = 3;
                if (player.position.y > (PADDLE_HEIGHT / 2) + 2) {
                        player.position.y -= player.speed;
                }
        }
        else if (analog_values->JoystickY > 3000) {
                player.speed = 2;
                if (player.position.y > (PADDLE_HEIGHT / 2) + 1) {
                        player.position.y -= player.speed;
                }
        }
        else if (analog_values->JoystickY > 2500) {
                player.speed = 1;
                if (player.position.y > (PADDLE_HEIGHT / 2)) {
                        player.position.y -= player.speed;
                }
        }
        else if (analog_values->JoystickY < 500) {
                player.speed = 3;
                if (player.position.y < BOARD_HEIGHT - (PADDLE_HEIGHT / 2) - 2) {
                        player.position.y += player.speed;
                }
        }
        else if (analog_values->JoystickY < 1000) {
                player.speed = 2;
                if (player.position.y < BOARD_HEIGHT - (PADDLE_HEIGHT / 2) - 1) {
                        player.position.y += player.speed;
                }
        }
        else if (analog_values->JoystickY < 1500)
        {
                player.speed = 1;
                if (player.position.y < BOARD_HEIGHT - (PADDLE_HEIGHT / 2)) {
                        player.position.y += player.speed;
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
        ball.position.x += ball.velocity.x * ball.speed;
        ball.position.y += ball.velocity.y * ball.speed;
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
        }
        else if (ball.position.y >= BOARD_HEIGHT - 1) {
                ball.position.y = BOARD_HEIGHT - 1;
                ball.velocity.y = -ball.velocity.y;
        }
}

//*****************************************************************************
//
// Checks if the ball is hitting a paddle
//
//*****************************************************************************
void check_paddle_collision(void) {
        // Player paddle
        if (ball.velocity.x < 0 && ball.position.x <= player.position.x + PADDLE_WIDTH && 
        ball.position.y >= player.position.y - (PADDLE_HEIGHT / 2) && 
        ball.position.y <= player.position.y + (PADDLE_HEIGHT / 2)) {
                ball.position.x = player.position.x + PADDLE_WIDTH;
                ball.velocity.x = -ball.velocity.x;
          }

        // Computer paddle
        if (ball.velocity.x > 0 &&
        ball.position.x >= computer.position.x - 1 &&
        ball.position.y >= computer.position.y - (PADDLE_HEIGHT / 2) &&
        ball.position.y <= computer.position.y + (PADDLE_HEIGHT / 2)) {
                ball.position.x = computer.position.x - 1;
                ball.velocity.x = -ball.velocity.x;
        }
}

//*****************************************************************************
//
// Move the computer, mirrowing the height of the ball
//
//*****************************************************************************
void move_computer(void) {
        if (ball.position.y < computer.position.y) {
                if (computer.position.y > PADDLE_HEIGHT / 2) {
                        computer.position.y--;
                }
        }
        else if (ball.position.y > computer.position.y) {
                if (computer.position.y < BOARD_HEIGHT - (PADDLE_HEIGHT / 2) - 1) {
                        computer.position.y++;
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

        ball.velocity.x = -1;
        ball.velocity.y = 1;
        ball.speed = 1;
}

//*****************************************************************************
//
// Setup the game.
//
//*****************************************************************************
void init_game() {
        player_score = 0;
        computer_score = 0;
        player.position.x = 0;
        player.position.y = BOARD_HEIGHT / 2;
        player.speed = 0;

        computer.position.x = BOARD_WIDTH - PADDLE_WIDTH;
        computer.position.y = BOARD_HEIGHT / 2;

        reset_ball();
	running = true;
}

//*****************************************************************************
//
// Check if ball went past players.
//
//*****************************************************************************
void check_score(void) {
        if (ball.position.x < 0) {
                computer_score++;
                reset_ball();
        }
        else if (ball.position.x >= BOARD_WIDTH) {
                player_score++;
                reset_ball();
        }
}

//*****************************************************************************
//
// Move the snake and check if game is done.
//
//*****************************************************************************
void update_game(void) {
        move_ball();

        check_wall_collision();

        check_paddle_collision();

        move_computer();

        check_score();
}
