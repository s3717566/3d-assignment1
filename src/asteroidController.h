#pragma once
#include "essentials.h"
#include "displayController.h"
#include "shipController.h"
#include "arenaController.h"

typedef struct {
	int id;
	coord pos;
	coord oldpos;
	coord futurepos;
	circle_coord_array outline;
	circle_coord_array outline_visual;
	float direction;
	float velocity_multiplier;
	float rotation_velocity_multiplier;
	float rotation;
	float radius;
	bool initialised;
	bool active;
	bool split;
	int hp;
	arena_collision last_hit_wall;
} asteroid;


#define MAX_ASTEROID_LIMIT 50
#define TICKS_BETWEEN_WAVES 5;
#define MIN_ASTEROID_SPEED 0.8;

void initialise_asteroid_controller();
void asteroid_controller();
void asteroid_controller_afterlife();
void initialise_asteroid(float x, float y, float direction, float radius, bool split);
void new_asteroid();
void split_asteroid(asteroid* ast);
void asteroid_movement(asteroid* ast);
void asteroid_to_string(asteroid* ast);
bool asteroid_out_of_bounds_check(asteroid* ast);
void ship_collision();
void damage_asteroid(asteroid* ast);
void bullet_collision();
void restart();
void has_died();
void rotate_asteroid(asteroid* ast);
void asteroid_bouncing(asteroid* ast);

extern bool restart_toggle;
extern bool game_over;

