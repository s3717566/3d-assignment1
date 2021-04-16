#pragma once

#include "displayController.h"
#include "essentials.h"
#include "arenaController.h"

#define SPEED_MULTIPLIER 4
#define TURN_MULTIPLIER 1.5
#define SHIP_HITBOX_RADIUS 20
#define MAX_PARTICLES 50
#define PARTICLE_INITIAL_SIZE 10
#define PARTICLE_LIFESPAN 100
#define PARTICLE_SPEED_MULTIPLIER 2
#define PARTICLE_FREQUENCY 10
#define MAX_BULLETS 10
#define FIRING_RATE 2
#define BULLET_SPEED_MULTIPLIER 8
#define ACCELERATION_RATE 1

typedef struct {
	//int s;										// size
	float x_pos, y_pos, old_x_pos, old_y_pos;		// position
	float red, green, blue;									// colour
	float direction;								// direction facing (out of 360 degrees)
	float velocity;									// velocity in pixels per second
	float rotation;
	float radius;
} ship_object;

typedef struct {
	coord pos, old_pos;      // position
	float red, green, blue;   // colour
	float direction;         // direction facing (out of 360 degrees)
	float velocity;           // velocity in pixels per second
	float radius;
	int lifespan;
	circle_coord_array cca;
} particle;

typedef struct {
	coord pos, old_pos;      // position
	float r, g, b;   // colour
	float direction;         // direction facing (out of 360 degrees)
	float v;           // velocity in pixels per second
	bool active;           // velocity in pixels per second
} bullet;

void ship_controller();
void ship_controller_afterlife();
void ship_init();
void ship_movement();
void initialise_ship_circles();
void ship_death();
bool death_check(circle_coord_array* cca);
void particle_movement(particle* part);
void particle_controller();
void initialise_particle(particle* particle);
void launch_particle();
void launch_bullet();
void initialise_bullet(bullet* bullet);
void bullet_movement(bullet* bullet);
void bullet_controller();
void kill_bullet(int index);
bool check_ship_death();
void restart_ship();
void acceleration_controller();

extern bool moving_forward;
extern bool turning_left;
extern bool turning_right;
extern bool fire_bullet;

ship_object ship_obj;
bullet active_bullets[MAX_BULLETS];









