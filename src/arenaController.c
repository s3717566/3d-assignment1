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