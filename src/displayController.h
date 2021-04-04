#pragma once

#include "circle.h"

#define ASTEROID_POINTS 20

void draw_arena(int, int);
void draw_ship(float, float, float, float);
void arena_warning(bool);
void draw_circle(circle_coord_array* cca, int circle_points);

extern int g_screen_width;
extern int g_screen_height;