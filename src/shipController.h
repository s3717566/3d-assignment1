#pragma once

#include "displayController.h"
#include "essentials.h"
#include "arenaController.h"

#define SPEED_MULTIPLIER 4
#define SHIP_HITBOX_RADIUS 20

typedef struct {
	//int s;           // size
	float xpos, ypos, oldxpos, oldypos;      // position
	float r, g, b;   // colour
	float direction;         // direction facing (out of 360 degrees)
	float v;           // velocity in pixels per second
	float rotation;
} ship_object;

void ship_controller();
void ship_init();
void ship_movement();
void initialise_ship_circles();
void ship_death();
bool death_check(circle_coord_array* cca);

bool moving_forward;
bool turning_left;
bool turning_right;

ship_object ship_obj;
circle_coord_array ship_hitbox_circle;
circle_coord_array ship_warning_circle;

int SHIP_HITBOX_SIZE = 60;
int SHIP_EDGE_WARNING_SIZE = 200;




