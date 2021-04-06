#pragma once
#include "essentials.h"
#include "displayController.h"

typedef enum arena_collision
{
	north,
	south,
	east,
	west,
	none
} arena_collision;

arena_collision arena_border_collision(circle_coord_array* cca);
bool out_of_bounds(float x, float y, float radius);
