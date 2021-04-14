#include "arenaController.h"

arena_collision arena_border_collision(float x, float y, float radius)
{
	 if (x - radius < -g_screen_width / 2)
	 	return west;
	
	 if (x + radius > g_screen_width / 2)
	 	return east;
	
	 if (y + radius > g_screen_height / 2)
	 	return north;
	
	 if (y - radius < -g_screen_height / 2) 
	 	return south;
	
	return none;
}

bool out_of_bounds(float x, float y, float radius)
{
	return
		((x - radius > g_screen_width / 2) //right side
			|| (x + radius < -(g_screen_width / 2)) //left side
			|| (y - radius > g_screen_height / 2) //top side
			|| (y + radius < -(g_screen_height / 2))); //bottom side 
}