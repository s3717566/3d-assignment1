#pragma once
#include "circle.h"
#include "displayController.h"

#define MAX_PARTICLES 50
#define PARTICLE_INITIAL_SIZE 10
#define PARTICLE_LIFESPAN 100
#define PARTICLE_SPEED_MULTIPLIER .5
#define PARTICLE_FREQUENCY 10

typedef struct {
	coord pos, old_pos;      // position
	float red, green, blue;   // colour
	float direction;         // direction facing (out of 360 degrees)
	float velocity;           // velocity in pixels per second
	float radius;
	int lifespan;
	circle_coord_array cca;
} particle;

void particle_ship_movement(particle* part);
void particle_controller();
void initialise_particle(particle* particle, float direction, float x, float y);
void launch_particle(float direction, float x, float y);