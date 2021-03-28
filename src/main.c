#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#if _WIN32
# include <windows.h>
#endif
#if __APPLE__
# include <OpenGL/gl.h>
# include <OpenGL/glu.h>
# include <GLUT/glut.h>
#else
# include <GL/gl.h>
# include <GL/glu.h>
# include <GL/glut.h>
#endif

#define KEY_ESC 27
int g_screen_width = 0;
int g_screen_height = 0;
float g_last_time = 0.0;

boolean moving_forward = false;
boolean turning_left = false;
boolean turning_right = false;

#define MOVING_RIGHT 1
#define MOVING_LEFT  -1
#define ROTATEDELTA 2

#define M_PI 3.14159265358979323846
#define CIRCLE_POINTS 100
#define MAX_ASTEROID_LIMIT 5

#define SHIP_HITBOX_RADIUS 20

#define SPEED_MULTIPLIER 4

#define SECONDS_BETWEEN_WAVES 10;


// DO NOT name your members like this!
typedef struct {
	int s;           // size
	float x, y;      // position
	float r, g, b;   // colour
	int m;           // movement direction
	int v;           // velocity in pixels per second
	float rotation;
} game_object_t;

typedef struct {
	//int s;           // size
	float xpos, ypos, oldxpos, oldypos;      // position
	float r, g, b;   // colour
	float direction;         // direction facing (out of 360 degrees)
	int v;           // velocity in pixels per second
	float rotation;
} ship_object;

typedef struct {
	float xpos, ypos;
} coord;

typedef struct {
	coord upper[CIRCLE_POINTS + 1];
	coord lower[CIRCLE_POINTS + 1];
} circle_coord_array;

typedef struct {
	coord pos;
	coord oldpos;
	circle_coord_array outline;
	float direction;
	float velocity_multiplier;
	float radius;
	int spawn_protection;
	boolean active;
} asteroid;

asteroid active_asteroids[MAX_ASTEROID_LIMIT];

#define NUM_OBJECTS 1
game_object_t g_game_objects[NUM_OBJECTS];
ship_object ship_obj;
circle_coord_array ship_hitbox_circle;
circle_coord_array ship_warning_circle;
circle_coord_array asteroid_spawning_circle;
int active_asteroids_count = 0;
int frames_until_next_wave = 0;
int wave_number = 5;

float red, green = 0;
float blue = 1;

bool warning = false;

void game_object_init(game_object_t* obj, int smin, int smax);
void ship_init(ship_object* obj);
void draw_arena();
void draw_ship();
void arena_warning(circle_coord_array* cca);
void check_collision(circle_coord_array* cca1, circle_coord_array* cca2);
void initialise_asteroid(asteroid* asteroid_in);
void asteroid_controller();
void initialise_circle(float radius, circle_coord_array* cca, float initialX, float initialY);
void draw_circle(circle_coord_array* cca);
void move_circle(circle_coord_array* cca, float DistTravelledX, float DistTravelledY);
void on_key_press(unsigned char key, int x, int y);
void on_key_release(unsigned char key, int x, int y);
void on_reshape(int w, int h);
void ship_controller();
void on_display();
void update_game_state(game_object_t* go, float dt);
void ship_movement();
void asteroid_movement(asteroid* ast);
void asteroid_to_string(asteroid* ast);
void render_frame(game_object_t* go);
void on_idle();
void init_app(int* argcp, char** argv);
int main(int argc, char** argv);
boolean asteroid_death_check(asteroid* ast);


void game_object_init(game_object_t* obj, int smin, int smax)
{
	obj->s = smin + (rand() % (smax - smin));
	obj->x = rand() % (g_screen_width - obj->s) + obj->s / 2;
	obj->y = rand() % (g_screen_height - obj->s) + obj->s / 2;
	obj->r = (float)rand() / RAND_MAX;
	obj->g = (float)rand() / RAND_MAX;
	obj->b = (float)rand() / RAND_MAX;
	obj->m = MOVING_RIGHT;
	obj->v = rand() % g_screen_width;
	obj->rotation = rand() % 360;
}

void ship_init(ship_object* obj) {
	obj->xpos = -600;
	obj->ypos = -300;
	obj->oldxpos = -600;
	obj->oldypos = -300;
	obj->r = (float)rand() / RAND_MAX;
	obj->g = (float)rand() / RAND_MAX;
	obj->b = (float)rand() / RAND_MAX;
	obj->direction = 0;
	obj->v = 0;
}

void draw_arena() {
	glPushMatrix();
	glColor3f(red, green, blue);
	glLineWidth(100);

	glBegin(GL_LINE_LOOP);

	glVertex2f(-(g_screen_width / 2), -(g_screen_height / 2));
	glVertex2f(g_screen_width / 2, -(g_screen_height / 2));
	glVertex2f(g_screen_width / 2, (g_screen_height / 2));
	glVertex2f(-(g_screen_width / 2), g_screen_height / 2);

	glEnd();
	glPopMatrix();
}

void draw_ship() {
	glPushMatrix();
	glTranslatef(ship_obj.xpos, ship_obj.ypos, ship_obj.v);
	glRotatef(-ship_obj.direction, 0, 0, 1);
	glBegin(GL_TRIANGLE_STRIP);
	glVertex2f(0, 120);
	glVertex2f(0, 0);
	glVertex2f(40, -20);
	glEnd();

	glBegin(GL_TRIANGLE_STRIP);
	glVertex2f(0, 120);
	glVertex2f(0, 0);
	glVertex2f(-40, -20);
	glEnd();
	glPopMatrix();
}

void arena_warning(circle_coord_array* cca) {
	if (!warning) {
		for (int i = 0; i < CIRCLE_POINTS; i++) {
			if (cca->upper[i].xpos > g_screen_width / 2
				|| cca->upper[i].xpos < -(g_screen_width / 2)
				|| cca->upper[i].ypos > g_screen_height / 2) {
				warning = true;
			}
		}

		for (int i = CIRCLE_POINTS; i > 0; i--) {
			if (cca->lower[i].ypos < -(g_screen_height / 2)) {
				warning = true;
			}
		}
	}
	else {
		for (int i = 0; i < CIRCLE_POINTS; i++) {
			if (cca->upper->xpos < g_screen_width / 2
				&& cca->upper->xpos > -(g_screen_width / 2)
				&& cca->upper->ypos < g_screen_height / 2) {
				warning = false;
			}
		}

		for (int i = CIRCLE_POINTS; i > 0; i--) {
			if (cca->lower->ypos > -(g_screen_height / 2)) {
				warning = false;
			}
		}
	}
	if (warning) {
		red = 1;
		blue = 0;
		green = 0;
	}
	else {
		red = 0;
		blue = 1;
		green = 0;
	}
}

void check_collision(circle_coord_array* cca1, circle_coord_array* cca2) {


}

void initialise_asteroid(asteroid* asteroid_in) {
	printf("initialised asteroid\n");
	asteroid_in->radius = 20 + rand() % 100;
	asteroid_in->velocity_multiplier = 0.1 + (rand() % 30 * 0.1);
	asteroid_in->spawn_protection = 500;
	asteroid_in->active = true;

	//deciding where to spawn on the spawn circle
	float x, y = 0;
	if (rand() % 1 == 0) {
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

	initialise_circle(asteroid_in->radius, &asteroid_in->outline, x, y);
}

void asteroid_controller() {
	if (active_asteroids_count == 0) {
		if (frames_until_next_wave <= 0) {
			//start wave
			wave_number++;
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
	draw_circle(&asteroid_spawning_circle);

	for (int i = 0; i < active_asteroids_count; i++) {
		printf(active_asteroids[i].active);
		if (&active_asteroids[i].active == true) {
			asteroid_movement(&active_asteroids[i]);
			draw_circle(&active_asteroids[i].outline);
			move_circle(&active_asteroids[i].outline, active_asteroids[i].oldpos.xpos - active_asteroids[i].pos.xpos, active_asteroids[i].oldpos.ypos - active_asteroids[i].pos.ypos);
			asteroid_death_check(&active_asteroids[i]);
		}
	}
}

boolean asteroid_death_check(asteroid* ast) {
	if (ast->spawn_protection <= 0 && ((ast->pos.xpos - ast->radius > g_screen_width / 2) //right side
		|| (ast->pos.xpos + ast->radius < -(g_screen_width / 2)) //left side
		|| (ast->pos.ypos - ast->radius > g_screen_height / 2) //top side
		|| (ast->pos.ypos + ast->radius < -(g_screen_height / 2)))) //bottom side 
	{
		printf("Boom... asteroid destroyed at %f, %f, width: %i, height: %i", ast->pos.xpos, ast->pos.ypos, g_screen_width, g_screen_height);

		//active_asteroids_count--;
		ast->active = false;
		return true;
	}
	return false;
}


void asteroid_to_string(asteroid* ast) {
	printf("\nradius: %f\nxpos: %f, ypos: %f\nvelocity: %f\ndirection: %f\n", ast->radius, ast->pos.xpos, ast->pos.ypos, ast->velocity_multiplier, ast->direction);
}

void initialise_circle(float radius, circle_coord_array* cca, float initialX, float initialY) {

	printf("Initialised hitbox");

	float x, y;

	//top semicircle
	for (int i = 0; i < CIRCLE_POINTS; i++) {
		x = ((i / (float)CIRCLE_POINTS - 0.5) * 2.0) * radius;
		y = sqrt(radius * radius - x * x);
		cca->upper[i].xpos = x + initialX;
		cca->upper[i].ypos = y + initialY;
	}

	//bottom semicircle
	for (int i = CIRCLE_POINTS; i > 0; i--) {
		x = (i / (float)CIRCLE_POINTS - 0.5) * 2.0 * radius;
		y = -sqrt(radius * radius - x * x);
		cca->lower[i].xpos = x + initialX;
		cca->lower[i].ypos = y + initialY;
	}
}

void draw_circle(circle_coord_array* cca) {
	glColor3f(1.0, 1.0, 1.0);
	glLineWidth(2.0);
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < CIRCLE_POINTS; i++) {
		glVertex2f(cca->upper[i].xpos, cca->upper[i].ypos);
	}

	for (int i = CIRCLE_POINTS; i > 0; i--) {
		glVertex2f(cca->lower[i].xpos, cca->lower[i].ypos);
	}
	glEnd();
}

void move_circle(circle_coord_array* cca, float DistTravelledX, float DistTravelledY) {
	for (int i = 0; i < CIRCLE_POINTS; i++) {
		cca->upper[i].xpos = cca->upper[i].xpos - DistTravelledX;
		cca->upper[i].ypos = cca->upper[i].ypos - DistTravelledY;
	}

	for (int i = CIRCLE_POINTS; i > 0; i--) {
		cca->lower[i].xpos = cca->lower[i].xpos - DistTravelledX;
		cca->lower[i].ypos = cca->lower[i].ypos - DistTravelledY;
	}
}

void on_key_press(unsigned char key, int x, int y)
{
	fprintf(stderr, "on_key_press()\n");
	switch (key) {
	case KEY_ESC:
		exit(EXIT_SUCCESS);
		break;
	case 'w':
		moving_forward = true;
		break;
	case 'a':
		turning_left = true;
		turning_right = false;
		break;
	case 'd':
		turning_left = false;
		turning_right = true;
		break;
	default:
		break;
	}
}

void on_key_release(unsigned char key, int x, int y)
{
	fprintf(stderr, "on_key_release()\n");
	switch (key) {
	case 'w':
		moving_forward = false;
		break;
	case 'a':
		turning_left = false;
		break;
	case 'd':
		turning_right = false;
		break;
	default:
		break;
	}
}

void on_reshape(int w, int h)
{
	g_screen_width = w;
	g_screen_height = h;

	fprintf(stderr, "on_reshape(%d, %d)\n", w, h);
	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-(w / 2), w / 2, -(h / 2), h / 2, -1.0, 1.0);


	for (int i = 0; i < NUM_OBJECTS; i++) {
		game_object_init(&g_game_objects[i], 100, 300);
	}
	ship_init(&ship_obj);
	initialise_circle(80, &ship_hitbox_circle, ship_obj.xpos, ship_obj.ypos + 20);
	initialise_circle(300, &ship_warning_circle, ship_obj.xpos, ship_obj.ypos + 20);
	initialise_circle(600, &asteroid_spawning_circle, 0, 0); //default: 1500
	draw_circle(&ship_hitbox_circle);
	//initialise_asteroid(&active_asteroids[0]);
	//initialise_circle(active_asteroids[0].radius, &active_asteroids[0].outline, active_asteroids[0].pos.xpos, active_asteroids[0].pos.ypos);
}

void ship_controller() {
	draw_ship();
	draw_circle(&ship_hitbox_circle);
	move_circle(&ship_hitbox_circle, ship_obj.oldxpos - ship_obj.xpos, ship_obj.oldypos - ship_obj.ypos);

	draw_circle(&ship_warning_circle);
	move_circle(&ship_warning_circle, ship_obj.oldxpos - ship_obj.xpos, ship_obj.oldypos - ship_obj.ypos);

	//wide radius around ship for warning near border
	arena_warning(&ship_warning_circle);
	//debug
	draw_circle(&ship_warning_circle);
}

void on_display()
{
	//fprintf(stderr, "on_display()\n");
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	render_frame(g_game_objects);

	int err;
	while ((err = glGetError()) != GL_NO_ERROR)
		printf("error: %s\n", gluErrorString(err));

	glutSwapBuffers();
}

void update_game_state(game_object_t* go, float dt)
{
	//fprintf(stderr, "dt=%f\n", dt);
	for (int i = 0; i < NUM_OBJECTS; i++) {
		float new_x = go[i].x + go[i].m * go[i].v * dt;
		if ((new_x + go[i].s / 2) > g_screen_width || new_x < 0 + go[i].s / 2) {
			go[i].m *= -1;
		}
		else {
			go[i].x = new_x;
		}
	}

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

void asteroid_movement(asteroid* ast) {
	//printf("asteroid is moving weee");
	ast->oldpos.xpos = ast->pos.xpos;
	ast->oldpos.ypos = ast->pos.ypos;

	ast->pos.xpos += sin(M_PI * ast->direction / 180) * ast->velocity_multiplier;
	ast->pos.ypos += cos(M_PI * ast->direction / 180) * ast->velocity_multiplier;

	ast->spawn_protection--;
}

void render_frame(game_object_t* go)
{
	draw_arena();

	ship_controller();
	asteroid_controller();
}

void on_idle()
{
	float cur_time = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
	float dt = cur_time - g_last_time;
	update_game_state(g_game_objects, dt);
	ship_movement();
	//asteroid_movement(&active_asteroids[0]);
	g_last_time = cur_time;
	glutPostRedisplay();
}

void init_app(int* argcp, char** argv)
{
	glutInit(argcp, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow("Tutorial 3");
	glutFullScreen();
	glutReshapeFunc(on_reshape);
	glutKeyboardFunc(on_key_press);
	glutKeyboardUpFunc(on_key_release);
	glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);

	glutDisplayFunc(on_display);
	glutIdleFunc(on_idle);

	g_last_time = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
	// *** DO NOT call game_object_init() here. WHY?
}

int main(int argc, char** argv)
{
	init_app(&argc, argv);
	glutMainLoop();
	return(EXIT_SUCCESS);
}
