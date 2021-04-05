#include "asteroidController.h"

int ASTEROID_MIN_SIZE = 60;
int ASTEROID_MAX_SIZE = 120;
int ASTEROID_SPAWN_RADIUS = 1500;

asteroid active_asteroids[MAX_ASTEROID_LIMIT];

circle_coord_array asteroid_spawning_circle;
int active_asteroids_count = 0;
int frames_until_next_wave = 0;
int wave_number = 0;

void initialise_asteroid_controller()
{
	initialise_circle(ASTEROID_SPAWN_RADIUS, &asteroid_spawning_circle, 0, 0, CIRCLE_POINTS, false); //default: 1500
	draw_circle(&asteroid_spawning_circle, false);
}

void initialise_asteroid(asteroid* asteroid_in) {
	printf("initialised asteroid\n");
	asteroid_in->radius = ASTEROID_MIN_SIZE + rand() % ASTEROID_MAX_SIZE;
	asteroid_in->velocity_multiplier = 0.3 + (rand() % 30 * 0.1);
	asteroid_in->spawn_protection = 500;
	asteroid_in->active = true;

	//deciding where to spawn on the spawn circle
	float x, y = 0;
	if (rand() % 2 == 0) {
		//upper hemisphere
		int index = rand() % 99;
		x = asteroid_spawning_circle.upper[index].xpos;
		y = asteroid_spawning_circle.upper[index].ypos;
	}
	else {
		//lower hemisphere
		int index = rand() % 99;
		x = asteroid_spawning_circle.lower[index].xpos;
		y = asteroid_spawning_circle.lower[index].ypos;
	}
	printf("x: %f, y: %f", x, y);
	asteroid_in->pos.xpos = x;
	asteroid_in->pos.ypos = y;

	//determine direction towards ship (could this be used to determine collisions?)
	float delta_x = x - ship_obj.xpos;
	float delta_y = y - ship_obj.ypos;
	float direction_rad = atan2(delta_y, delta_x);
	printf("angle: %f", direction_rad);

	asteroid_in->direction = -90 + -direction_rad * (180 / M_PI);
	active_asteroids_count++;

	initialise_circle(asteroid_in->radius, &asteroid_in->outline, x, y, CIRCLE_POINTS, false);
	initialise_circle(asteroid_in->radius, &asteroid_in->outline_visual, x, y, ASTEROID_POINTS, true);
}

void asteroid_controller() {
	int temp = 0;
	for (int i = 0; i < active_asteroids_count; i++) {
		if (active_asteroids[i].active == true) {
			temp++;
		}
	}


	if (temp == 0) {
		if (frames_until_next_wave <= 0) {
			//start wave
			wave_number++;
			active_asteroids_count = 0;
			printf("launching wave %i", wave_number);

			for (int i = 0; i < wave_number && i < MAX_ASTEROID_LIMIT; i++) {
				printf("initialising asteroid with index: %i\n", i);
				initialise_asteroid(&active_asteroids[i]);
				asteroid_to_string(&active_asteroids[i]);
			}
			frames_until_next_wave = 60 * SECONDS_BETWEEN_WAVES; //CHANGE THIS - how many frames in a second?
		}
		else {
			frames_until_next_wave--;
		}

	}

	//debug - does not need to be drawn

	for (int i = 0; i < active_asteroids_count; i++) {
		if (active_asteroids[i].active == true) {
			asteroid_movement(&active_asteroids[i]);

			//collision checking for asteroids with each other
			for (int j = 0; j < active_asteroids_count; j++)
			{
				if (i != j) {
					if (active_asteroids[j].active == true) {
						if (check_collision(active_asteroids[i].pos.xpos, active_asteroids[i].pos.ypos, active_asteroids[j].pos.xpos, active_asteroids[j].pos.ypos, active_asteroids[i].radius, active_asteroids[j].radius))
						{
							printf("boing");
						}
					}
				}
			}

			move_circle(&active_asteroids[i].outline_visual, active_asteroids[i].oldpos.xpos - active_asteroids[i].pos.xpos, active_asteroids[i].oldpos.ypos - active_asteroids[i].pos.ypos, ASTEROID_POINTS);
			move_circle(&active_asteroids[i].outline, active_asteroids[i].oldpos.xpos - active_asteroids[i].pos.xpos, active_asteroids[i].oldpos.ypos - active_asteroids[i].pos.ypos, CIRCLE_POINTS);

			draw_circle(&active_asteroids[i].outline_visual, ASTEROID_POINTS);
			//draw_circle(&active_asteroids[i].outline, CIRCLE_POINTS);

			asteroid_out_of_bounds_check(&active_asteroids[i]);
		}
	}

	ship_collision();
	
}

bool asteroid_out_of_bounds_check(asteroid* ast) {
	if (ast->spawn_protection <= 0)
		if (out_of_bounds(ast->pos.xpos, ast->pos.ypos, ast->radius))
	{
		printf("Boom... asteroid destroyed at %f, %f, width: %i, height: %i", ast->pos.xpos, ast->pos.ypos, g_screen_width, g_screen_height);

		ast->active = false;
		return true;
	}
	return false;
}


void asteroid_to_string(asteroid* ast) {
	printf("\nradius: %f\nxpos: %f, ypos: %f\nvelocity: %f\ndirection: %f\n", ast->radius, ast->pos.xpos, ast->pos.ypos, ast->velocity_multiplier, ast->direction);
}

void asteroid_movement(asteroid* ast) {
	//printf("asteroid is moving weee");
	ast->oldpos.xpos = ast->pos.xpos;
	ast->oldpos.ypos = ast->pos.ypos;

	ast->pos.xpos += sin(M_PI * ast->direction / 180) * ast->velocity_multiplier;
	ast->pos.ypos += cos(M_PI * ast->direction / 180) * ast->velocity_multiplier;

	ast->spawn_protection--;
}

void ship_collision()
{
	for (int i = 0; i < active_asteroids_count; i++)
	{
		//change below - this means spawn protected asteroids wont destroy the ship
			//if (active_asteroids[i].active == true) {
		if (check_collision(active_asteroids[i].pos.xpos, active_asteroids[i].pos.ypos, ship_obj.xpos, ship_obj.ypos, active_asteroids[i].radius, ship_obj.r))
		{
			printf("boom");
			ship_death();
		}
	}
}