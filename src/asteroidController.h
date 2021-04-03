#pragma once
#include "essentials.h"
#include "displayController.h"
#include "shipController.h"

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

asteroid active_asteroids[MAX_ASTEROID_LIMIT];

int ASTEROID_MIN_SIZE = 60;
int ASTEROID_MAX_SIZE = 120;
int ASTEROID_SPAWN_RADIUS = 1500;

circle_coord_array asteroid_spawning_circle;
int active_asteroids_count = 0;
int frames_until_next_wave = 0;
int wave_number = 0;

void initialise_asteroid_controller();
void asteroid_controller();
void initialise_asteroid(asteroid* asteroid_in);
void asteroid_movement(asteroid* ast);
void asteroid_to_string(asteroid* ast);
bool asteroid_out_of_bounds_check(asteroid* ast);
void ship_collision();


