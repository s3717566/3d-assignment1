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
	float xpos, ypos;      // position
	float r, g, b;   // colour
	float direction;         // direction facing (out of 360 degrees)
	int v;           // velocity in pixels per second
	float rotation;
} ship_object;

#define NUM_OBJECTS 1
game_object_t g_game_objects[NUM_OBJECTS];
ship_object ship_obj;


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
	obj->xpos = 100;
	obj->ypos = 100;
	obj->r = (float)rand() / RAND_MAX;
	obj->g = (float)rand() / RAND_MAX;
	obj->b = (float)rand() / RAND_MAX;
	obj->direction = 0;
	obj->v = 0;
}

void draw_arena() {
	glColor3f(0, 0, 255);
	glLineWidth(100);

	glBegin(GL_LINE_LOOP);
	/*glVertex2f(-100, -100);
	glVertex2f(100, -100);
	glVertex2f(100, 100);
	glVertex2f(-100, 100);*/

	glVertex2f(-(g_screen_width / 2), -(g_screen_height / 2));
	glVertex2f(g_screen_width / 2, -(g_screen_height / 2));
	glVertex2f(g_screen_width / 2, (g_screen_height / 2));
	glVertex2f(-(g_screen_width / 2), g_screen_height / 2);
	glEnd();
}

void draw_ship() {
	glPushMatrix();
	glTranslatef(ship_obj.xpos, ship_obj.ypos, ship_obj.v);
	glRotatef(-ship_obj.direction,0,0,1);
	glBegin(GL_TRIANGLE_STRIP);
	glVertex2f(0, 120); //vertex 1
	glVertex2f(0, 0); //vertex 2
	glVertex2f(40, -20); //vertex 3
	glEnd();

	glBegin(GL_TRIANGLE_STRIP);
	glVertex2f(0, 120); //vertex 1
	glVertex2f(0, 0); //vertex 2
	glVertex2f(-40, -20); //vertex 3
	glEnd();
	glPopMatrix();
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
	fprintf(stderr, "on_reshape(%d, %d)\n", w, h);
	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-(w / 2), w / 2, -(h /2), h / 2, -1.0, 1.0);

	g_screen_width = w;
	g_screen_height = h;
	for (int i = 0; i < NUM_OBJECTS; i++) {
		game_object_init(&g_game_objects[i], 100, 300);
	}
	ship_init(&ship_obj);
}

void render_frame(game_object_t* go)
{
	draw_arena();
	draw_ship();
	for (int i = 0; i < NUM_OBJECTS; i++) {
		glPushMatrix();
		glTranslatef(go[i].x, go[i].y, 1);
		glScalef(go[i].s, go[i].s, 1);
		go[i].rotation += rand() % ROTATEDELTA;
		glRotatef(go[i].rotation, 0, 0, 1);
		glColor3f(go[i].r, go[i].g, go[i].b);
		glBegin(GL_LINE_LOOP);
		glVertex2f(-0.5, -0.5);
		glVertex2f(0.5, -0.5);
		glVertex2f(0.5, 0.5);
		glVertex2f(-0.5, 0.5);
		glEnd();
		glPopMatrix();
	}
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
	if (moving_forward) {
		//ship_obj.ypos += 1;
		ship_obj.xpos += sin(M_PI * ship_obj.direction / 180);
		ship_obj.ypos += cos(M_PI * ship_obj.direction / 180);
	}
	if (turning_left) {
		ship_obj.direction--;
	}
	if (turning_right) {
		ship_obj.direction++;
	}
	//printf("angle: " + ship_obj.direction);
}

void on_idle()
{
	float cur_time = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
	float dt = cur_time - g_last_time;
	update_game_state(g_game_objects, dt);
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
