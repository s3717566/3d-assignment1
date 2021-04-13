#include "shipController.h"

bool moving_forward = false;
bool turning_left = false;
bool turning_right = false;
bool fire_bullet = false;
bool dead = false;

int SHIP_HITBOX_SIZE = 60;
int SHIP_EDGE_WARNING_SIZE = 200;
int INITIAL_PARTICLE_SIZE = 10;
int current_particle = 0;
int particle_cooldown = 0;
int bullet_cooldown = 0;

circle_coord_array ship_hitbox_circle;
circle_coord_array ship_warning_circle;
particle active_particles[MAX_PARTICLES];


void ship_controller() {
	draw_ship(ship_obj.xpos, ship_obj.ypos, ship_obj.direction);
	ship_movement();

	move_circle(&ship_hitbox_circle, ship_obj.oldxpos - ship_obj.xpos, ship_obj.oldypos - ship_obj.ypos, CIRCLE_POINTS);

	move_circle(&ship_warning_circle, ship_obj.oldxpos - ship_obj.xpos, ship_obj.oldypos - ship_obj.ypos, CIRCLE_POINTS);

	//display border warning
	if (arena_border_collision(ship_obj.xpos, ship_obj.ypos, ship_warning_circle.radius) != none)
	//if (out_of_bounds(ship_obj.xpos, ship_obj.ypos, SHIP_EDGE_WARNING_SIZE))
	{
		//printf("%i",arena_border_collision(&ship_warning_circle));
		arena_warning(true);
	}
	else
	{
		arena_warning(false);
	}

	//debug
	draw_circle(&ship_hitbox_circle, CIRCLE_POINTS, 0);
	draw_circle(&ship_warning_circle, CIRCLE_POINTS, 0);

	if (death_check(&ship_hitbox_circle)) {
		ship_death();
	}
}

void ship_init() {
	ship_obj.xpos = 00;
	ship_obj.ypos = 00;
	ship_obj.oldxpos = 00;
	ship_obj.oldypos = 00;
	ship_obj.r = (float)rand() / RAND_MAX;
	ship_obj.g = (float)rand() / RAND_MAX;
	ship_obj.b = (float)rand() / RAND_MAX;
	ship_obj.direction = 0;
	ship_obj.v = 0;
	dead = false;
}

void ship_movement() {
	ship_obj.oldxpos = ship_obj.xpos;
	ship_obj.oldypos = ship_obj.ypos;

	if (moving_forward) {
		ship_obj.xpos += sin(M_PI * ship_obj.direction / 180) * SPEED_MULTIPLIER;
		ship_obj.ypos += cos(M_PI * ship_obj.direction / 180) * SPEED_MULTIPLIER;
		launch_particle();
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

void particle_controller()
{
	//get point of back of ship
	//ensure point rotates when ship does
	//while (moving forward)
		//loop through all particles
			//move particle to new x and y based on configurable velocity and -direction of ship
			//make smaller
			//reduce lifespan
			//when lifespan = 0
				//set x and y of particle back to ship, with small x range
	//dont draw if not (moving forward)
	for (int i = 0; i < MAX_PARTICLES; i++)
	{
		//TEMP POSITIONING
		if (active_particles[i].lifespan > 0) 
		{
			particle_movement(&active_particles[i]);
			//initialise_circle needs to be used here as a circle cant just "change size", a new circle must be drawn with a new radius at the spot of the old one.
			initialise_circle(active_particles[i].radius, &active_particles[i].cca, active_particles[i].pos.xpos, active_particles[i].pos.ypos, CIRCLE_POINTS, false);
			//move_circle(&active_particles[i].cca, active_particles[i].old_pos.xpos - active_particles[i].pos.xpos, active_particles[i].old_pos.ypos - active_particles[i].pos.ypos, CIRCLE_POINTS);
			draw_circle(&active_particles[i].cca, CIRCLE_POINTS, 0);
		}
	}
}

void particle_movement(particle* ast) {
	//printf("asteroid is moving weee");
	ast->old_pos.xpos = ast->pos.xpos;
	ast->old_pos.ypos = ast->pos.ypos;

	ast->pos.xpos += sin(M_PI * ast->direction / 180) * ast->v;
	ast->pos.ypos += cos(M_PI * ast->direction / 180) * ast->v;
	
	ast->radius *= 0.95;

	ast->lifespan--;
}

void launch_particle()
{
	if (particle_cooldown >= 0)
	{
		initialise_particle(&active_particles[current_particle]);
		particle_cooldown = 100;
		current_particle++;
		if (current_particle == MAX_PARTICLES - 1)
		{
			current_particle = 0;
		}
		
	} else
	{
		particle_cooldown -= PARTICLE_FREQUENCY;
	}
}

void initialise_particle(particle* particle)
{
	particle->pos.xpos = ship_obj.xpos - 10 + (rand() % 20);
	particle->pos.ypos = ship_obj.ypos - 10;

	particle->radius = PARTICLE_INITIAL_SIZE;
	particle->direction = 180 + ship_obj.direction;
	particle->lifespan = PARTICLE_LIFESPAN;
	particle->v = PARTICLE_SPEED_MULTIPLIER;

	initialise_circle(particle->radius, &particle->cca, particle->pos.xpos, particle->pos.ypos, CIRCLE_POINTS, false);
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

	bullet_cooldown -= FIRING_RATE;	
}

void initialise_bullet(bullet* bullet)
{
	bullet->pos.xpos = ship_obj.xpos;
	bullet->pos.ypos = ship_obj.ypos + 40;
	
	bullet->direction = ship_obj.direction;
	bullet->v = 1 * BULLET_SPEED_MULTIPLIER;

	bullet->active = true;

	draw_point(bullet->pos.xpos, bullet->pos.ypos);
}

void bullet_movement(bullet* bullet) {
	//TODO: old_pos doesnt seem to be needed
	bullet->old_pos.xpos = bullet->pos.xpos;
	bullet->old_pos.ypos = bullet->pos.ypos;

	bullet->pos.xpos += sin(M_PI * bullet->direction / 180) * bullet->v;
	bullet->pos.ypos += cos(M_PI * bullet->direction / 180) * bullet->v;

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

