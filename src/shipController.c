#include "shipController.h"

bool moving_forward = false;
bool turning_left = false;
bool turning_right = false;
bool fire_bullet = false;
bool dead = false;

int SHIP_HITBOX_SIZE = 40;
int SHIP_EDGE_WARNING_SIZE = 200;
int bullet_cooldown = 0;

circle_coord_array ship_hitbox_circle;
circle_coord_array ship_warning_circle;

void ship_controller() {
	draw_ship(ship_obj.x_pos, ship_obj.y_pos, ship_obj.direction);
	ship_movement();

	move_circle(&ship_hitbox_circle, ship_obj.old_x_pos - ship_obj.x_pos, ship_obj.old_y_pos - ship_obj.y_pos, CIRCLE_POINTS);

	move_circle(&ship_warning_circle, ship_obj.old_x_pos - ship_obj.x_pos, ship_obj.old_y_pos - ship_obj.y_pos, CIRCLE_POINTS);

	//display border warning

	arena_warning(arena_border_collision(ship_obj.x_pos, ship_obj.y_pos, ship_warning_circle.radius));

	//debug
	//draw_circle(&ship_hitbox_circle, CIRCLE_POINTS, 0);
	//draw_circle(&ship_warning_circle, CIRCLE_POINTS, 0);

	if (arena_border_collision(ship_obj.x_pos, ship_obj.y_pos, ship_obj.radius) != none)
	{
		printf("You have hit an arena wall");
		ship_death();
	}
}

void ship_controller_afterlife()
{
	draw_ship(ship_obj.x_pos, ship_obj.y_pos, ship_obj.direction++);
}


void ship_init() {
	ship_obj.x_pos = -600;
	ship_obj.y_pos = -300;
	ship_obj.old_x_pos = -600;
	ship_obj.old_y_pos = -300;

	ship_obj.red = (float)rand() / RAND_MAX;
	ship_obj.green = (float)rand() / RAND_MAX;
	ship_obj.blue = (float)rand() / RAND_MAX;
	ship_obj.direction = 45;
	ship_obj.velocity = 0;
	ship_obj.radius = SHIP_HITBOX_SIZE;
	dead = false;
}

void acceleration_controller()
{
	if (moving_forward && ship_obj.velocity < 1)
	{
		if (ship_obj.velocity < 1)
			ship_obj.velocity += ACCELERATION_RATE * 0.01;
	}
	else
	{
		if (ship_obj.velocity > 0)
			ship_obj.velocity -= ACCELERATION_RATE * 0.01;
		if (ship_obj.velocity < 0)
		{
			ship_obj.velocity = 0;
		}
	}
}

void ship_movement() {
	ship_obj.old_x_pos = ship_obj.x_pos;
	ship_obj.old_y_pos = ship_obj.y_pos;

	acceleration_controller();

	ship_obj.x_pos += sin(M_PI * ship_obj.direction / 180) * ship_obj.velocity * SPEED_MULTIPLIER * delta_time;
	ship_obj.y_pos += cos(M_PI * ship_obj.direction / 180) * ship_obj.velocity * SPEED_MULTIPLIER * delta_time;
	
	if (moving_forward) {
		launch_particle(180 + ship_obj.direction, ship_obj.x_pos, ship_obj.y_pos);
	}
	
	if (turning_left) {
		ship_obj.direction -= TURN_MULTIPLIER * delta_time;
	}
	if (turning_right) {
		ship_obj.direction += TURN_MULTIPLIER * delta_time;
	}
	//printf("angle: " + ship_obj.direction);
}

void initialise_ship_circles()
{
	initialise_circle(SHIP_HITBOX_SIZE, &ship_hitbox_circle, ship_obj.x_pos, ship_obj.y_pos, CIRCLE_POINTS, false);
	initialise_circle(SHIP_EDGE_WARNING_SIZE, &ship_warning_circle, ship_obj.x_pos, ship_obj.y_pos, CIRCLE_POINTS, false);
}

void ship_death()
{
	//printf("you have died.");
	dead = true;
}

void restart_ship()
{
	set_reset_time();
	ship_init();
	initialise_ship_circles();
}

bool death_check(circle_coord_array* cca)
{
	if (arena_border_collision(cca->center.xpos, cca->center.ypos, cca->radius) != none)
	{
		//TODO: NEED TO RESET ASTEROID CONTROLLER ON DEATHS FROM THIS CLASS
		//printf("arena border collision %i", arena_border_collision(ship_obj.xpos, ship_obj.ypos, ship_warning_circle.radius));
		return true;
	}
	return false;
}

void launch_bullet()
{
	if (fire_bullet) {
		if (bullet_cooldown <= 0)
		{
			bool fired = false;
			for (int i = 0; i < MAX_BULLETS; i++)
			{
				if (!active_bullets[i].active && !fired)
				{
					initialise_bullet(&active_bullets[i]);
					fired = true;
				}
			}
			bullet_cooldown = 100;
			//fire_bullet = false;

		}
	}

	bullet_cooldown -= FIRING_RATE * delta_time;
}

void initialise_bullet(bullet* bullet)
{
	bullet->pos.xpos = ship_obj.x_pos;
	bullet->pos.ypos = ship_obj.y_pos;

	bullet->direction = ship_obj.direction;
	bullet->v = 1 * BULLET_SPEED_MULTIPLIER;
	bullet->r = 5;

	bullet->active = true;

	draw_point(bullet->pos.xpos, bullet->pos.ypos);
}

void bullet_movement(bullet* bullet) {
	bullet->pos.xpos += sin(M_PI * bullet->direction / 180) * bullet->v * delta_time;
	bullet->pos.ypos += cos(M_PI * bullet->direction / 180) * bullet->v * delta_time;

	if (out_of_bounds(bullet->pos.xpos, bullet->pos.ypos, bullet->r))
	{
		bullet->active = false;
	}
}

void bullet_controller()
{
	launch_bullet();
	for (int i = 0; i < MAX_BULLETS; i++)
	{
		if (active_bullets[i].active)
		{
			bullet_movement(&active_bullets[i]);
			draw_point(active_bullets[i].pos.xpos, active_bullets[i].pos.ypos);
		}
	}
}

void kill_bullet(int index)
{
	active_bullets[index].active = false;
}

bool check_ship_death()
{
	return dead;
}

