/*
 * game.h
 *
 * Created: 26-05-2018 15:01:45
 *  Author: amavin
 */ 


#ifndef GAME_H_
#define GAME_H_
#include <stdint.h>
#include <stdbool.h>
#define GAME_RENDERER_TASK_PERIOD 50
#define SCREEN_DIMENSION_X 14
#define SCREEN_DIMENSION_Y 10
#define STEPS_PER_PIXEL 20
#define MOVESPEED_PER_FOOD 1

extern uint8_t rng_seed;

typedef enum
{
	EMPTY = 0,
	D_DOWN = 1,
	D_UP = 3,
	D_LEFT = 4,
	D_RIGHT = 6,
	TAIL = 7,
	I_OFFSET = 5,
	J_OFFSET = 2,
	PL_TWO_OFFSET = 8,
	FOOD = 16,
} Direction_t;
typedef enum
{
	PL_ONE = 0,
	PL_TWO = 1,
} Player_t;
typedef struct snake
{
	uint8_t steps;
	Direction_t direction;
	uint8_t movespeed;
	uint8_t length;
	uint8_t pixels[20];
	uint8_t max_length;
} snake_t;
int16_t mod(int16_t a, int16_t b);
void restart_game();
void snake_change_direction(Player_t player, Direction_t direction);
uint8_t xy_to_pixel_id(uint8_t x, uint8_t y);
void game_renderer_task(void *pvParameters);
bool check_collisions_for_player(Player_t player, snake_t **snakes);
snake_t* snake_new_instance();
void init_game(snake_t **snakes);
void clear_screen(uint16_t* framebuffer);
uint8_t move_player_step(Player_t player, snake_t **snakes, uint8_t food_location);
uint8_t get_new_position(uint8_t origin_pixelid, Direction_t direction, uint8_t steps);
uint8_t y_offset(uint8_t pixel_id);
uint8_t x_offset(uint8_t pixel_id);
void draw_game(uint16_t* framebuffer, snake_t **snakes, uint8_t food_location);
#endif /* GAME_H_ */