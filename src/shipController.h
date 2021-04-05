#pragma once

#include "displayController.h"
#include "essentials.h"
#include "arenaController.h"

#define SPEED_MULTIPLIER 4
#define SHIP_HITBOX_RADIUS 20
#define MAX_PARTICLES 20
#define PARTICLE_INITIAL_SIZE 10
#define PARTICLE_LIFESPAN 100
#define PARTICLE_SPEED_MULTIPLIER 1
#define PARTICLE_FREQUENCY 5

typedef struct {
	//int s;           // size
	float xpos, ypos, oldxpos, oldypos;      // position
	float r, g, b;   // colour
	float direction;         // direction facing (out of 360 degrees)
	float v;           // velocity in pixels per second
	float rotation;
} ship_object;

typedef struct {
	//int s;           // size
	coord pos, old_pos;      // position
	float r, g, b;   // colour
	float direction;         // direction facing (out of 360 degrees)
	float v;           // velocity in pixels per second
	float radius;
	int lifespan;
	circle_coord_array cca;
} particle;

void ship_controller();
void ship_init();
void ship_movement();
void initialise_ship_circles();
void ship_death();
bool death_check(circle_coord_array* cca);
void particle_movement(particle* ast);
void particle_controller();
void initialise_particle(particle* particle);
void launch_particle();

extern bool moving_forward;
extern bool turning_left;
extern bool turning_right;

ship_object ship_obj;









