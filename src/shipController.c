#include "shipController.h"

void ship_controller() {
	draw_ship(ship_obj.xpos, ship_obj.ypos, ship_obj.v, ship_obj.direction);
	ship_movement();

	move_circle(&ship_hitbox_circle, ship_obj.oldxpos - ship_obj.xpos, ship_obj.oldypos - ship_obj.ypos, CIRCLE_POINTS);

	move_circle(&ship_warning_circle, ship_obj.oldxpos - ship_obj.xpos, ship_obj.oldypos - ship_obj.ypos, CIRCLE_POINTS);

	//display border warning
	if (arena_border_collision(&ship_warning_circle))
	{
		arena_warning(true);
	}
	else
	{
		arena_warning(false);
	}

	//debug
	draw_circle(&ship_hitbox_circle, CIRCLE_POINTS);
	draw_circle(&ship_warning_circle, CIRCLE_POINTS);

	

	if (death_check(&ship_hitbox_circle)) {
		ship_death();
	}
}

void ship_init() {
	ship_obj.xpos = -600;
	ship_obj.ypos = -300;
	ship_obj.oldxpos = -600;
	ship_obj.oldypos = -300;
	ship_obj.r = (float)rand() / RAND_MAX;
	ship_obj.g = (float)rand() / RAND_MAX;
	ship_obj.b = (float)rand() / RAND_MAX;
	ship_obj.direction = 0;
	ship_obj.v = 0;
}

void ship_movement() {
	ship_obj.oldxpos = ship_obj.xpos;
	ship_obj.oldypos = ship_obj.ypos;

	if (moving_forward) {
		ship_obj.xpos += sin(M_PI * ship_obj.direction / 180) * SPEED_MULTIPLIER;
		ship_obj.ypos += cos(M_PI * ship_obj.direction / 180) * SPEED_MULTIPLIER;
	}
	if (turning_left) {
		ship_obj.direction--;
	}
	if (turning_right) {
		ship_obj.direction++;
	}
	//printf("angle: " + ship_obj.direction);
}

void initialise_ship_circles()
{
	initialise_circle(SHIP_HITBOX_SIZE, &ship_hitbox_circle, ship_obj.xpos, ship_obj.ypos, CIRCLE_POINTS, false);
	initialise_circle(SHIP_EDGE_WARNING_SIZE, &ship_warning_circle, ship_obj.xpos, ship_obj.ypos, CIRCLE_POINTS, false);
}

void ship_death()
{
	printf("you have died.");
}

bool death_check(circle_coord_array* cca)
{
	if (arena_border_collision(cca))
	{
		return true;
	}
}