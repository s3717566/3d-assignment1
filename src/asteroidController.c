#include "asteroidController.h"

int ASTEROID_MIN_SIZE = 40;
int ASTEROID_MAX_SIZE = 100;
//int ASTEROID_SPAWN_RADIUS = 1500;  //default: 1500

asteroid active_asteroids[MAX_ASTEROID_LIMIT];

circle_coord_array asteroid_spawning_circle;
int active_asteroids_count = 0;
int seconds_until_next_wave = 0;
int wave_number = 0;
int score = 0;
int cur_time = 0;
bool game_over = false;
bool restart_toggle = false;
int id = 0;
int wave_end_time = 0;

void initialise_asteroid_controller()
{
	//initiate spawning circle: adding 20% to account for corners
	int asteroid_spawning_circle_radius = (arena_width / 2) + (arena_width / 5);
	initialise_circle(asteroid_spawning_circle_radius, &asteroid_spawning_circle, 0, 0, CIRCLE_POINTS, false);
	score = 0;
	wave_number = 0;
	active_asteroids_count = 0;
	seconds_until_next_wave = 0;

	//reset asteroids
	for (int i = 0; i < MAX_ASTEROID_LIMIT; i++)
	{
		active_asteroids[i].active = false;
	}
}

void new_asteroid()
{
	//deciding where to spawn on the spawn circle
	float x, y = 0;
	if (rand() % 2 == 0) {
		//upper hemisphere
		int index = 1 + rand() % CIRCLE_POINTS - 1;
		x = asteroid_spawning_circle.upper[index].xpos;
		y = asteroid_spawning_circle.upper[index].ypos;
	}
	else {
		//lower hemisphere
		int index = 1 + rand() % CIRCLE_POINTS - 1;
		x = asteroid_spawning_circle.lower[index].xpos;
		y = asteroid_spawning_circle.lower[index].ypos;
	}

	//determine direction towards ship (could this be used to determine collisions?)
	float delta_x = x - ship_obj.x_pos;
	float delta_y = y - ship_obj.y_pos;
	float direction_rad = atan2(delta_y, delta_x);
	//printf("angle: %f", direction_rad);

	float direction = -90 + -direction_rad * (180 / M_PI);
	float radius = ASTEROID_MIN_SIZE + rand() % ASTEROID_MAX_SIZE;

	initialise_asteroid(x, y, direction, radius, false);
}

void split_asteroid(asteroid* ast)
{
	if (!ast->split) {
		float x;
		float y;
		float direction;
		float radius;

		radius = ast->radius / 2;

		//first asteroid
		x = ast->pos.xpos + (ast->radius * 1.1);
		y = ast->pos.ypos + (ast->radius * 1.1);
		direction = ast->direction + 90;
		initialise_asteroid(x, y, direction, radius, true);

		//second asteroid
		x = ast->pos.xpos - (ast->radius * 1.1);
		y = ast->pos.ypos - (ast->radius * 1.1);
		direction = ast->direction - 90;
		initialise_asteroid(x, y, direction, radius, true);
	}
	ast->active = false;
}

void initialise_asteroid(float x, float y, float direction, float radius, bool split) {
	//finding the next inactive asteroid
	asteroid* asteroid_in = 0;
	bool init = false;
	for (int i = 0; i < MAX_ASTEROID_LIMIT; i++)
	{
		if (!active_asteroids[i].active)
		{
			asteroid_in = &active_asteroids[i];
			init = true;
			break;
		}
	}

	//emergency catch
	if (!init)
		asteroid_in = &active_asteroids[0];

	asteroid_in->split = split;
	asteroid_in->direction = direction;
	asteroid_in->radius = radius;
	asteroid_in->id = id++;
	int max_ast_speed = MAX_ASTEROID_SPEED * 10;
	asteroid_in->velocity_multiplier = MIN_ASTEROID_SPEED + (rand() % max_ast_speed * 0.1);
	asteroid_in->active = true;
	asteroid_in->hp = (radius / 40);
	asteroid_in->initialised = false;
	asteroid_in->last_hit_wall = none;
	asteroid_in->rotation = 0;
	asteroid_in->rotation_velocity_multiplier = (rand() % 200) * 0.01 - 1;
	asteroid_in->time_spawned = get_time();
	asteroid_in->pos.xpos = x;
	asteroid_in->pos.ypos = y;
	asteroid_in->oldpos.xpos = x;
	asteroid_in->oldpos.ypos = y;
	asteroid_in->futurepos.xpos = x;
	asteroid_in->futurepos.ypos = y;

	active_asteroids_count++;

	initialise_circle(asteroid_in->radius, &asteroid_in->outline, x, y, CIRCLE_POINTS, false);
	initialise_circle(asteroid_in->radius, &asteroid_in->outline_visual, x, y, ASTEROID_POINTS, true);
}

void asteroid_controller() {
	//if there are no more active asteroids this wave, then the wave is over
	bool wave_over = true;
	for (int i = 0; i < active_asteroids_count; i++) {
		if (active_asteroids[i].active == true) {
			wave_over = false;
		}
	}

	if (wave_over) {
		if (wave_end_time == 0)
		{
			wave_end_time = get_time();
		}
		//initialise wave
		if (wave_end_time + SECONDS_BETWEEN_WAVES < get_time()) {
			wave_end_time = 0;
			wave_number++;
			active_asteroids_count = 0;
			printf("launching wave %i", wave_number);

			for (int i = 0; i < wave_number && i < MAX_ASTEROID_LIMIT; i++) {
				new_asteroid();
				asteroid_to_string(&active_asteroids[i]);
			}
		}
	}

	for (int i = 0; i < active_asteroids_count; i++) {
		if (active_asteroids[i].active == true) {
			asteroid_movement(&active_asteroids[i]);

			move_circle(&active_asteroids[i].outline_visual, 
				active_asteroids[i].oldpos.xpos - active_asteroids[i].pos.xpos, 
				active_asteroids[i].oldpos.ypos - active_asteroids[i].pos.ypos, ASTEROID_POINTS);
			move_circle(&active_asteroids[i].outline, 
				active_asteroids[i].oldpos.xpos - active_asteroids[i].pos.xpos, 
				active_asteroids[i].oldpos.ypos - active_asteroids[i].pos.ypos, CIRCLE_POINTS);
			draw_circle(&active_asteroids[i].outline_visual, ASTEROID_POINTS, active_asteroids[i].rotation);
			draw_circle(&asteroid_spawning_circle, CIRCLE_POINTS, 0);

			//Initialises asteroids once they are in the arena
			if (arena_border_collision(active_asteroids[i].futurepos.xpos, active_asteroids[i].futurepos.ypos, active_asteroids[i].radius) == none)
			{
				active_asteroids[i].initialised = true;
			}

			//if an asteroid has entered the arena or 10 seconds has elapsed
			//printf("time spawned: %i, current_time: %i", active_asteroids[i].time_spawned, get_time());
			if (active_asteroids[i].initialised || active_asteroids[i].time_spawned + 5 < get_time()) {
				//when an asteroid sneakily escapes
				asteroid_out_of_bounds_check(&active_asteroids[i]);
				
				asteroid_bouncing(&active_asteroids[i]);
			}
		}
	}

	if (!game_over) {
		ship_collision();
		bullet_collision();

		if (check_ship_death())
		{
			printf("check_ship_death()");
			has_died();
		}
	}

	string_controller();
	
}

void string_controller()
{
	char score_str[10] = "Score: ";
	score_str[7] = score / 10 + '0';
	score_str[8] = score % 10 + '0';
	draw_string(-arena_width / 2 + 40, arena_height / 2 - 40, score_str);

	//now that this is fixed it probably doesnt need to be here
	cur_time = get_time();
	char time_str[10] = "Time: ";
	time_str[6] = cur_time / 100 + '0';
	time_str[7] = (cur_time % 100) / 10 + '0';
	time_str[8] = cur_time % 10 + '0';
	draw_string(arena_width / 2 - 140, arena_height / 2 - 40, time_str);
}

void asteroid_bouncing(asteroid* ast)
{
	//collision checking for asteroids with each other
	for (int j = 0; j < active_asteroids_count; j++)
	{
		if (ast->id != active_asteroids[j].id) {
			if (active_asteroids[j].active == true) {
				if (check_collision(ast->futurepos.xpos,
					ast->futurepos.ypos,
					active_asteroids[j].futurepos.xpos,
					active_asteroids[j].futurepos.ypos,
					ast->radius,
					active_asteroids[j].radius))
				{
					ast->direction += 180;
					active_asteroids[j].direction += 180;
					ast->last_hit_wall = none;
					active_asteroids[j].last_hit_wall = none;
				}
			}
		}
	}

	//keeps direction within positive 360 when doing calculations
	ast->direction = fmod(ast->direction, 360);
	if (ast->direction < 0)
	{
		ast->direction = 360 + ast->direction;
	}

	//collision with arena walls
	switch (arena_border_collision(ast->futurepos.xpos, ast->futurepos.ypos, ast->radius))
	{
	case north:
		if (ast->last_hit_wall != north) {
			ast->last_hit_wall = north;
			ast->direction = 180 - (fmod(ast->direction, 360));
		}
		break;
	case south:

		if (ast->last_hit_wall != south) {
			ast->last_hit_wall = south;
			ast->direction = 180 - (fmod(ast->direction, 360));
		}
		break;
	case east:
		if (ast->last_hit_wall != east) {
			ast->last_hit_wall = east;
			ast->direction = 360 - fmod(ast->direction, 360);
		}
		break;
	case west:
		if (ast->last_hit_wall != west) {
			ast->last_hit_wall = west;
			ast->direction = 360 - fmod(ast->direction, 360);
		}
		break;
	case none:
		break;
	}
}

void restart()
{
	game_over = false;
	restart_ship();
	initialise_asteroid_controller();
}

bool asteroid_out_of_bounds_check(asteroid* ast) {
	if (out_of_bounds(ast->pos.xpos, ast->pos.ypos, ast->radius))
	{
		printf("Boom... asteroid destroyed at %f, %f, width: %i, height: %i", ast->pos.xpos, ast->pos.ypos, arena_width, arena_height);

		ast->active = false;
		return true;
	}
	return false;
}

void asteroid_to_string(asteroid* ast) {
	printf("\nradius: %f\nxpos: %f, ypos: %f\nvelocity: %f\ndirection: %f\nrotation: %f", ast->radius, ast->pos.xpos, ast->pos.ypos, ast->velocity_multiplier, ast->direction, ast->rotation);
}

void asteroid_movement(asteroid* ast) {
	ast->oldpos.xpos = ast->pos.xpos;
	ast->oldpos.ypos = ast->pos.ypos;

	ast->pos.xpos += sin(M_PI * ast->direction / 180) * ast->velocity_multiplier * delta_time;
	ast->pos.ypos += cos(M_PI * ast->direction / 180) * ast->velocity_multiplier * delta_time;

	ast->futurepos.xpos += sin(M_PI * ast->direction / 180) * ast->velocity_multiplier * delta_time;
	ast->futurepos.ypos += cos(M_PI * ast->direction / 180) * ast->velocity_multiplier * delta_time;

	ast->outline.center.xpos = ast->pos.xpos;
	ast->outline.center.ypos = ast->pos.ypos;

	ast->outline_visual.center.xpos = ast->pos.xpos;
	ast->outline_visual.center.ypos = ast->pos.ypos;

	rotate_asteroid(ast);
}

void rotate_asteroid(asteroid* ast)
{
	if (ast->rotation_velocity_multiplier > 0)
		ast->rotation += 1 + ast->rotation_velocity_multiplier;
	else
		ast->rotation -= 1 + ast->rotation_velocity_multiplier;
}

void ship_collision()
{
	for (int i = 0; i < active_asteroids_count; i++)
	{
		if (active_asteroids[i].initialised) {
			if (active_asteroids[i].active) {
				if (check_collision(active_asteroids[i].pos.xpos, active_asteroids[i].pos.ypos,
					ship_obj.x_pos, ship_obj.y_pos,
					active_asteroids[i].radius, ship_obj.radius))
				{
					has_died();
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
		split_asteroid(ast);
		for (int i = 0; i < 50; i++)
			launch_particle(rand() % 360, ast->pos.xpos, ast->pos.ypos);
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
					if (check_collision(active_bullets[i].pos.xpos, active_bullets[i].pos.ypos,
						active_asteroids[j].pos.xpos, active_asteroids[j].pos.ypos,
						5, active_asteroids[i].radius))
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

void has_died()
{
	game_over = true;
	printf("game over");
	restart_toggle = false;
	ship_death();
}