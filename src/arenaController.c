#include "arenaController.h"

bool arena_border_collision(circle_coord_array* cca)
{
	bool collided = false;
	for (int i = 0; i < CIRCLE_POINTS; i++) {
		if (cca->upper[i].xpos > g_screen_width / 2
			|| cca->upper[i].xpos < -(g_screen_width / 2)
			|| cca->upper[i].ypos > g_screen_height / 2) {
			collided = true;
		}
	}

	for (int i = CIRCLE_POINTS; i > 0; i--) {
		if (cca->lower[i].ypos < -(g_screen_height / 2)) {
			collided = true;
		}
	}
	return collided;
}

bool out_of_bounds(float x, float y, float radius)
{
	//printf("out of bounds");
	return
		((x - radius > g_screen_width / 2) //right side
			|| (x + radius < -(g_screen_width / 2)) //left side
			|| (y - radius > g_screen_height / 2) //top side
			|| (y + radius < -(g_screen_height / 2))); //bottom side 
}