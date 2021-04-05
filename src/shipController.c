#include "shipController.h"

bool moving_forward = false;
bool turning_left = false;
bool turning_right = false;

int SHIP_HITBOX_SIZE = 60;
int SHIP_EDGE_WARNING_SIZE = 200;
int INITIAL_PARTICLE_SIZE = 10;
int current_particle = 0;
int particle_cooldown = 0;

circle_coord_array ship_hitbox_circle;
circle_coord_array ship_warning_circle;
particle active_particles[MAX_PARTICLES];


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
	printf("you have died.");
}

bool death_check(circle_coord_array* cca)
{
	if (arena_border_collision(cca))
	{
		return true;
	}
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
			initialise_circle(active_particles[i].radius, &active_particles[i].cca, active_particles[i].pos.xpos, active_particles[i].pos.ypos, CIRCLE_POINTS, false);
			//move_circle(&active_particles[i].cca, active_particles[i].old_pos.xpos - active_particles[i].pos.xpos, active_particles[i].old_pos.ypos - active_particles[i].pos.ypos, CIRCLE_POINTS);
			draw_circle(&active_particles[i].cca, CIRCLE_POINTS);
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

//Maybe not needed?
void align_thrusters()
{
	
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