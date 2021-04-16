#pragma once

#include "circle.h"

#define ASTEROID_POINTS 12
#define ARENA_WIDTH 1820
#define ARENA_HEIGHT 980

typedef enum arena_collision
{
	north,
	south,
	east,
	west,
	none
} arena_collision;

void draw_arena(int, int);
void draw_ship(float, float, float);
void arena_warning(arena_collision);
void draw_circle(circle_coord_array* cca, int circle_points, float rotation);
void draw_point(float x, float y);
void draw_string(float x, float y, char* string);
int get_time();
void set_reset_time();

extern int arena_width;
extern int arena_height;
