#include "arenaController.h"

arena_collision arena_border_collision(circle_coord_array* cca)
{
	// bool collided = false;
	// for (int i = 0; i < CIRCLE_POINTS; i++) {
	// 	if (cca->upper[i].xpos > g_screen_width / 2)
	// 	{
	// 		return east;
	// 	}
	//
	//
	// 	if (cca->upper[i].ypos > g_screen_height / 2)
	// 	{
	// 		return north;
	// 	}
	// }
	//
	// for (int i = CIRCLE_POINTS; i > 0; i--) {
	// 	if (cca->lower[i].xpos < -(g_screen_width / 2))
	// 	{
	// 		return west;
	// 	}
	// 	
	// 	if (cca->lower[i].ypos < -(g_screen_height / 2)) {
	// 		return south;
	// 	}
	// 	
	//
	// }

	 if (cca->center.xpos - cca->radius < -g_screen_width / 2)
	 {
	 	//printf("west xpos: %f, radius: %f, less than %i\n", cca->center.xpos, cca->radius, -(g_screen_width / 2 ));
	 	return west;
	 }
	
	 if (cca->center.xpos + cca->radius > g_screen_width / 2)
	 {
	 	return east;
	 }
	
	 if (cca->center.ypos + cca->radius > g_screen_height / 2)
	 {
	 	return north;
	 }
	
	 if (cca->center.ypos - cca->radius < -g_screen_height / 2) 
	 {
	 	return south;
	 }
	
	return none;
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