#pragma once

#include "essentials.h"

#define CIRCLE_POINTS 50

typedef struct {
	float xpos, ypos;
} coord;

typedef struct {
	coord upper[CIRCLE_POINTS + 1];
	coord lower[CIRCLE_POINTS + 1];
	coord center;
	float radius;
} circle_coord_array;

void move_circle(circle_coord_array*, float, float, int);
bool check_collision(float , float , float , float , float , float );
void initialise_circle(float, circle_coord_array*, float, float, int, bool);
