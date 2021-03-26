#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

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

#define MOVING_RIGHT 1
#define MOVING_LEFT  -1
#define ROTATEDELTA 2

// DO NOT name your members like this!
typedef struct {
	int s;           // size
	float x, y;      // position
	float r, g, b;   // colour
	int m;           // movement direction
	int v;           // velocity in pixels per second
	float rotation;
} game_object_t;

#define NUM_OBJECTS 1
game_object_t g_game_objects[NUM_OBJECTS];


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

void draw_arena() {
	printf("draw arena");
	glColor3f(0, 0, 255);

	glBegin(GL_LINE_LOOP);
	glPointSize(100);
	/*glVertex2f(-100, -100);
	glVertex2f(100, -100);
	glVertex2f(100, 100);
	glVertex2f(-100, 100);*/

	glVertex2f(-(g_screen_width / 2), -(g_screen_width / 2));
	glVertex2f(g_screen_width / 2, -(g_screen_width / 2));
	glVertex2f(g_screen_width / 2, (g_screen_width / 2));
	glVertex2f(-(g_screen_width / 2), g_screen_width / 2);
	glEnd();
}

void ship_init() {

}

void on_key_press(unsigned char key, int x, int y)
{
	fprintf(stderr, "on_key_press()\n");
	switch (key) {
	case KEY_ESC:
		exit(EXIT_SUCCESS);
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
	glOrtho(0.0, w, 0.0, h, -1.0, 1.0);

	g_screen_width = w;
	g_screen_height = h;
	for (int i = 0; i < NUM_OBJECTS; i++) {
		game_object_init(&g_game_objects[i], 100, 300);
	}
}

void render_frame(game_object_t* go)
{
	draw_arena();
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
