#pragma once
#include "essentials.h"
#include "displayController.h"

arena_collision arena_border_collision(float x, float y, float radius);
bool out_of_bounds(float x, float y, float radius);
