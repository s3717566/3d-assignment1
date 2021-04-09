#include "asteroidController.h"

int ASTEROID_MIN_SIZE = 40;
int ASTEROID_MAX_SIZE = 100;
int ASTEROID_SPAWN_RADIUS = 1500;  //default: 1500

asteroid active_asteroids[MAX_ASTEROID_LIMIT];

circle_coord_array asteroid_spawning_circle;
int active_asteroids_count = 0;
int frames_until_next_wave = 0;
int wave_number = 20;
int score = 0;
int cur_time = 0;

void initialise_asteroid_controller()
{
	initialise_circle(ASTEROID_SPAWN_RADIUS, &asteroid_spawning_circle, 0, 0, CIRCLE_POINTS, false);
	score = 0;
	wave_number = 0;
	active_asteroids_count = 0;
	frames_until_next_wave = 0;
	
	//reset asteroids
	for (int i = 0; i < MAX_ASTEROID_LIMIT; i++)
	{
		active_asteroids[i].active = false;
	}
}

void initialise_asteroid(asteroid* asteroid_in) {
	//printf("initialised asteroid\n");
	int r = ASTEROID_MIN_SIZE + rand() % ASTEROID_MAX_SIZE;
	asteroid_in->radius = r;
	asteroid_in->velocity_multiplier = 0.8 + (rand() % 30 * 0.1);
	asteroid_in->spawn_protection = 500;
	asteroid_in->active = true;
	asteroid_in->hp = (r / 40);
	asteroid_in->initialised = false;
	asteroid_in->last_hit_wall = none;
	asteroid_in->rotation = 0;
	asteroid_in->rotation_velocity_multiplier = (rand() % 200) * 0.01 - 1;

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

	asteroid_in->oldpos.xpos = x;
	asteroid_in->oldpos.ypos = y;

	//determine direction towards ship (could this be used to determine collisions?)
	float delta_x = x - ship_obj.xpos;
	float delta_y = y - ship_obj.ypos;
	float direction_rad = atan2(delta_y, delta_x);
	//printf("angle: %f", direction_rad);

	asteroid_in->direction = -90 + -direction_rad * (180 / M_PI);
	active_asteroids_count++;

	initialise_circle(asteroid_in->radius, &asteroid_in->outline, x, y, CIRCLE_POINTS, false);
	initialise_circle(asteroid_in->radius, &asteroid_in->outline_visual, x, y, ASTEROID_POINTS, true);
}

void asteroid_controller() {
	bool wave_over = true;
	for (int i = 0; i < active_asteroids_count; i++) {
		if (active_asteroids[i].active == true) {
			wave_over = false;
		}
	}


	if (wave_over) {
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
							//printf("boing");
						}
					}
				}
			}

			 move_circle(&active_asteroids[i].outline_visual, active_asteroids[i].oldpos.xpos - active_asteroids[i].pos.xpos, active_asteroids[i].oldpos.ypos - active_asteroids[i].pos.ypos, ASTEROID_POINTS);
			 move_circle(&active_asteroids[i].outline, active_asteroids[i].oldpos.xpos - active_asteroids[i].pos.xpos, active_asteroids[i].oldpos.ypos - active_asteroids[i].pos.ypos, CIRCLE_POINTS);

			draw_circle(&active_asteroids[i].outline_visual, ASTEROID_POINTS, active_asteroids[i].rotation);
			//draw_circle(&active_asteroids[i].outline, CIRCLE_POINTS, 0);

			if (arena_border_collision(&active_asteroids[i].outline) == none)
			{
				active_asteroids[i].initialised = true;
			}
			
			//this means that the asteroid has fully entered the field
			if (active_asteroids[i].initialised) {
				//asteroid_out_of_bounds_check(&active_asteroids[i]);

				//printf("initialised");

				switch (arena_border_collision(&active_asteroids[i].outline))
				{
				case north:
					if (active_asteroids[i].last_hit_wall != north) {
						active_asteroids[i].last_hit_wall = north;

						active_asteroids[i].direction = 180 - (fmod(active_asteroids[i].direction, 360));
					}
					break;
				case south:

					if (active_asteroids[i].last_hit_wall != south) {
						active_asteroids[i].last_hit_wall = south;

						active_asteroids[i].direction = 180 - (fmod(active_asteroids[i].direction, 360));
					}
					break;
				case east:
					if (active_asteroids[i].last_hit_wall != east) {
						active_asteroids[i].last_hit_wall = east;

						active_asteroids[i].direction = 360 - fabs(fmod(active_asteroids[i].direction, 360));
					}
					break;
				case west:
					if (active_asteroids[i].last_hit_wall != west) {
						//initialised asteroids going west will have negative direction
						//needs to be corrected to positive
						if (active_asteroids[i].direction < 0)
						{
							active_asteroids[i].direction = 360 + active_asteroids[i].direction;
						}
						active_asteroids[i].last_hit_wall = west;

						active_asteroids[i].direction = 360 - fabs(fmod(active_asteroids[i].direction, 360));
					}
					break;
				case none:
					break;
				}
			}
		}
	}

	ship_collision();
	bullet_collision();
	
	char score_str[10] = "Score: ";
	score_str[7] = score / 10 + '0';
	score_str[8] = score % 10 + '0';
	draw_string(-g_screen_width / 2 + 40, g_screen_height / 2 - 40, score_str);

	//now that this is fixed it probably doesnt need to be here
	cur_time = get_time();
	char time_str[10] = "Time: ";
	time_str[6] = cur_time / 100 + '0';
	time_str[7] = (cur_time % 100) / 10 + '0';
	time_str[8] = cur_time % 10 + '0';
	draw_string(-g_screen_width / 2 + 40, g_screen_height / 2 - 60, time_str);	
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
	printf("\nradius: %f\nxpos: %f, ypos: %f\nvelocity: %f\ndirection: %f\nrotation: %f", ast->radius, ast->pos.xpos, ast->pos.ypos, ast->velocity_multiplier, ast->direction, ast->rotation);
}

void asteroid_movement(asteroid* ast) {
	//printf("asteroid is moving weee");
	ast->oldpos.xpos = ast->pos.xpos;
	ast->oldpos.ypos = ast->pos.ypos;

	ast->pos.xpos += sin(M_PI * ast->direction / 180) * ast->velocity_multiplier;
	ast->pos.ypos += cos(M_PI * ast->direction / 180) * ast->velocity_multiplier;

	ast->outline.center.xpos = ast->pos.xpos;
	ast->outline.center.ypos = ast->pos.ypos;
	
	ast->outline_visual.center.xpos = ast->pos.xpos;
	ast->outline_visual.center.ypos = ast->pos.ypos;

	//TODO: replace spawn protection with a check that it has entered the arena (x and y within arena bounds)
	ast->spawn_protection--;

	if (ast->rotation_velocity_multiplier > 0)
	{
		ast->rotation += 1 + ast->rotation_velocity_multiplier;
	} else
	{
		ast->rotation -= 1 + ast->rotation_velocity_multiplier;
	}
}

void ship_collision()
{
	for (int i = 0; i < active_asteroids_count; i++)
	{
		//change below - this means spawn protected asteroids wont destroy the ship
			//if (active_asteroids[i].active == true) {
		if (active_asteroids[i].initialised) {
			if (active_asteroids[i].active) {
				if (check_collision(active_asteroids[i].pos.xpos, active_asteroids[i].pos.ypos, ship_obj.xpos, ship_obj.ypos, active_asteroids[i].radius, ship_obj.r))
				{
					printf("boom");
					ship_death();
					initialise_asteroid_controller();
				}
			}
		}
	}
}

void damage_asteroid(asteroid* ast)
{
	ast->hp--;
	if (ast->hp <= 0)
	{
		score++;
		ast->active = false;
	}
}

void bullet_collision()
{
	for (int i = 0; i < MAX_BULLETS; i++)
	{
		if (active_bullets[i].active)
		{
			for (int j = 0; j < MAX_ASTEROID_LIMIT; j++)
			{
				if (active_asteroids[j].active)
				{
					if (check_collision(active_bullets[i].pos.xpos, active_bullets[i].pos.ypos, active_asteroids[j].pos.xpos, active_asteroids[j].pos.ypos, 1, active_asteroids[i].radius))
					{
						damage_asteroid(&active_asteroids[j]);
						printf("hp left %i", active_asteroids[j].hp);
						kill_bullet(i);
					}
				}
			}
		}
	}
}