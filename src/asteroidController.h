#pragma once
#include "essentials.h"
#include "displayController.h"
#include "shipController.h"
#include "arenaController.h"

typedef struct {
	coord pos;
	coord oldpos;
	circle_coord_array outline;
	circle_coord_array outline_visual;
	float direction;
	float velocity_multiplier;
	float rotation_velocity_multiplier;
	float rotation;
	float radius;
	int spawn_protection;
	bool active;
} asteroid;

#define MAX_ASTEROID_LIMIT 10
#define SECONDS_BETWEEN_WAVES 10;

void initialise_asteroid_controller();
void asteroid_controller();
void initialise_asteroid(asteroid* asteroid_in);
void asteroid_movement(asteroid* ast);
void asteroid_to_string(asteroid* ast);
bool asteroid_out_of_bounds_check(asteroid* ast);
void ship_collision();


