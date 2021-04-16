#include "particleController.h"

particle active_particles[MAX_PARTICLES];
int INITIAL_PARTICLE_SIZE = 10;
int current_particle = 0;
int particle_cooldown = 0;

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
			particle_ship_movement(&active_particles[i]);
			//initialise_circle needs to be used here as a circle cant just "change size", a new circle must be drawn with a new radius at the spot of the old one.
			initialise_circle(active_particles[i].radius, &active_particles[i].cca, active_particles[i].pos.xpos, active_particles[i].pos.ypos, CIRCLE_POINTS, false);
			//move_circle(&active_particles[i].cca, active_particles[i].old_pos.xpos - active_particles[i].pos.xpos, active_particles[i].old_pos.ypos - active_particles[i].pos.ypos, CIRCLE_POINTS);
			draw_circle(&active_particles[i].cca, CIRCLE_POINTS, 0);
		}
	}
}

void particle_ship_movement(particle* part) {
	//printf("asteroid is moving weee");

	part->pos.xpos += sin(M_PI * part->direction / 180) * part->velocity * delta_time;
	part->pos.ypos += cos(M_PI * part->direction / 180) * part->velocity * delta_time;

	part->radius *= 0.95;

	part->lifespan--;
}

void launch_particle(float direction, float x, float y)
{
	if (particle_cooldown >= 0)
	{
		initialise_particle(&active_particles[current_particle], direction, x, y);
		particle_cooldown = 100;
		current_particle++;
		if (current_particle == MAX_PARTICLES - 1)
		{
			current_particle = 0;
		}

	}
	else
	{
		particle_cooldown -= PARTICLE_FREQUENCY;
	}
}

void initialise_particle(particle* particle, float direction, float x, float y)
{
	particle->pos.xpos = x - 10 + (rand() % 20);
	particle->pos.ypos = y - 10 + (rand() % 20);

	particle->radius = PARTICLE_INITIAL_SIZE;
	particle->direction = direction;
	particle->lifespan = PARTICLE_LIFESPAN;
	particle->velocity = PARTICLE_SPEED_MULTIPLIER;

	initialise_circle(particle->radius, &particle->cca, particle->pos.xpos, particle->pos.ypos, CIRCLE_POINTS, false);
}