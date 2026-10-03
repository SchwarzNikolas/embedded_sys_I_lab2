#include "maze.h"

Player player;
Goal goal;
bool won;
bool running;
MazeField maze[BOARD_ROWS][BOARD_COLUMNS] = {
        {WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL},
        {WALL,START,EMPTY,EMPTY,WALL,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,WALL,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,WALL,WALL},
        {WALL,WALL,WALL,EMPTY,WALL,EMPTY,WALL,WALL,WALL,EMPTY,WALL,EMPTY,WALL,EMPTY,WALL,WALL,WALL,WALL,EMPTY,WALL, WALL},
        {WALL,EMPTY,WALL,EMPTY,WALL,EMPTY,WALL,EMPTY,EMPTY,EMPTY,WALL,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,WALL,EMPTY,EMPTY,WALL,WALL},
        {WALL,EMPTY,EMPTY,EMPTY,WALL,EMPTY,WALL,EMPTY,WALL,EMPTY,WALL,WALL,WALL,WALL,WALL,EMPTY,WALL,WALL,WALL,EMPTY,WALL},
        {WALL,EMPTY,WALL,EMPTY,WALL,EMPTY,WALL,EMPTY,WALL,EMPTY,WALL,EMPTY,EMPTY,EMPTY,WALL,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,WALL},
        {WALL,EMPTY,WALL,WALL,WALL,EMPTY,WALL,EMPTY,WALL,WALL,WALL,EMPTY,WALL,EMPTY,WALL,WALL,WALL,WALL,WALL,EMPTY,WALL},
        {WALL,EMPTY,WALL,EMPTY,EMPTY,EMPTY,WALL,EMPTY,EMPTY,EMPTY,WALL,EMPTY,WALL,EMPTY,WALL,EMPTY,EMPTY,EMPTY,EMPTY,WALL,WALL},
        {WALL,EMPTY,WALL,WALL,WALL,EMPTY,WALL,WALL,WALL,EMPTY,WALL,EMPTY,WALL,EMPTY,WALL,EMPTY,WALL,WALL,WALL,WALL,WALL},
        {WALL,EMPTY,EMPTY,EMPTY,WALL,EMPTY,EMPTY,EMPTY,WALL,EMPTY,WALL,EMPTY,WALL,EMPTY,WALL,EMPTY,WALL,EMPTY,EMPTY,EMPTY,WALL},
        {WALL,EMPTY,WALL,WALL,WALL,WALL,WALL,EMPTY,WALL,EMPTY,WALL,EMPTY,WALL,EMPTY,WALL,EMPTY,EMPTY,EMPTY,WALL,WALL,WALL},
        {WALL,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,WALL,EMPTY,EMPTY,EMPTY,WALL,EMPTY,WALL,EMPTY,WALL,EMPTY,EMPTY,EMPTY,WALL},
        {WALL,EMPTY,WALL,WALL,WALL,WALL,WALL,WALL,WALL,EMPTY,WALL,WALL,WALL,EMPTY,WALL,EMPTY,WALL,EMPTY,WALL,EMPTY,WALL},
        {WALL,EMPTY,WALL,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,WALL,EMPTY,WALL,EMPTY,EMPTY,EMPTY,WALL,EMPTY,WALL,EMPTY,WALL,EMPTY,WALL},
        {WALL,EMPTY,WALL,WALL,WALL,EMPTY,WALL,EMPTY,WALL,EMPTY,WALL,EMPTY,WALL,WALL,WALL,EMPTY,WALL,WALL,WALL,EMPTY,WALL},
        {WALL,EMPTY,EMPTY,EMPTY,EMPTY,WALL,WALL,EMPTY,WALL,EMPTY,EMPTY,EMPTY,WALL,EMPTY,EMPTY,EMPTY,WALL,EMPTY,EMPTY,EMPTY,WALL},
        {WALL,WALL,WALL,WALL,WALL,WALL,WALL,EMPTY,WALL,WALL,EMPTY,WALL,WALL,EMPTY,WALL,WALL,WALL,EMPTY,WALL,EMPTY,WALL},
        {WALL,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,WALL,EMPTY,WALL,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,WALL,GOAL,WALL,EMPTY,EMPTY,EMPTY,WALL},
        {WALL,EMPTY,WALL,WALL,WALL,EMPTY,WALL,EMPTY,WALL,EMPTY,WALL,WALL,WALL,WALL,WALL,EMPTY,WALL,WALL,WALL,EMPTY,WALL},
        {WALL,EMPTY,EMPTY,EMPTY,WALL,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,WALL,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,WALL},
        {WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL}
};


//*****************************************************************************
//
// Setup the game.
//
//*****************************************************************************
void init_game() {;
	player.direction = NONE;
        player.position.x = 1;
        player.position.y = 1;
	won = 0;
	running = true;
}

//*****************************************************************************
//
// Parse the raw joystick values into directions.
//
//*****************************************************************************
void handle_joystick_input(AnalogValues *analog_values) {
	if (analog_values->JoystickY > 3000) {
                player.direction = UP;
	} else if (analog_values->JoystickY < 1000) {
                player.direction = DOWN;
	} else if (analog_values->JoystickX < 1000) {
                player.direction = LEFT;
	} else if (analog_values->JoystickX > 3000) {
		player.direction = RIGHT;
	} else {
                player.direction = NONE;
        }
}

//*****************************************************************************
//
// Handle the movement of the player.
//
//*****************************************************************************
void move_player(void) {
	Position new_pos;

	// calculate position of the player
	new_pos = player.position;
	switch (player.direction) {
                case UP:
                        new_pos.y--;
                        break;

                case DOWN:
                        new_pos.y++;
                        break;

                case LEFT:
                        new_pos.x--;
                        break;

                case RIGHT:
                        new_pos.x++;
                        break;
	}

	// move the player
	player.position = new_pos;
}

//*****************************************************************************
//
// Checks if player is in the wall or goal.
//
//*****************************************************************************
void check_collision() {
	// collision with wall, goal or empty
	if (maze[player.position.y][player.position.x] == WALL) {
		running = false;
	} else if (maze[player.position.y][player.position.x] == GOAL) {
		running = false;
                won = true;
	} else {
                maze[player.position.y][player.position.x] = VISITED;
        }
}

//*****************************************************************************
//
// Move the player and check if game is done.
//
//*****************************************************************************
void update_game() {
	move_player();
	check_collision();
}
