#include "displayController.h"

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

#include "circle.h"

int g_screen_width = 0;
int g_screen_height = 0;
int time_on_death = 0;

//arena_border colours
float red_north;
float red_south;
float red_east;
float red_west;
float blue_north;
float blue_south;
float blue_east;
float blue_west;

float ship_red = 0;
float ship_green = .5;
float ship_blue = .5;

float ship_outline_red = 0;
float ship_outline_green = .8;
float ship_outline_blue = .2;

void draw_arena(int screen_width, int screen_height) {
	glPushMatrix();
	glLineWidth(20);

	int err;
	while ((err = glGetError()) != GL_NO_ERROR)
		printf("display arena: %s\n", gluErrorString(err));

	//south wall
	glColor3f(red_south, 0, blue_south);
	glBegin(GL_LINE_LOOP);
	glVertex2f(-(screen_width / 2), -(screen_height / 2));
	glVertex2f(screen_width / 2, -(screen_height / 2));
	glEnd();

	//east wall
	glColor3f(red_east, 0, blue_east);
	glBegin(GL_LINE_LOOP);
	glVertex2f(screen_width / 2, -(screen_height / 2));
	glVertex2f(screen_width / 2, (screen_height / 2));
	glEnd();

	//north wall
	glColor3f(red_north, 0, blue_north);
	glBegin(GL_LINE_LOOP);
	glVertex2f(screen_width / 2, (screen_height / 2));
	glVertex2f(-(screen_width / 2), screen_height / 2);
	glEnd();

	//west wall
	glColor3f(red_west, 0, blue_west);
	glBegin(GL_LINE_LOOP);
	glVertex2f(-(screen_width / 2), screen_height / 2);
	glVertex2f(-(screen_width / 2), -(screen_height / 2));
	glEnd();

	glEnd();
	glPopMatrix();
}

void draw_ship(float x, float y, float direction) {

	glPushMatrix();

	glTranslatef(x, y, 0);
	glRotatef(-direction, 0, 0, 1);
	glTranslatef(-x, -y, 0);

	int err;
	while ((err = glGetError()) != GL_NO_ERROR)
		printf("display ship: %s\n", gluErrorString(err));

	glColor3f(ship_red, ship_green, ship_blue);
	glBegin(GL_TRIANGLE_STRIP);
	glVertex2f(x, y + 55);
	glVertex2f(x, y - 35);
	glVertex2f(x + 50, y - 55);
	glEnd();

	glBegin(GL_TRIANGLE_STRIP);
	glVertex2f(x, y + 55);
	glVertex2f(x, y - 35);
	glVertex2f(x - 50, y - 55);
	glEnd();

	//outline
	glColor3f(ship_outline_red, ship_outline_green, ship_outline_blue);
	glLineWidth(4);
	glBegin(GL_LINE_LOOP);
	glVertex2f(x, y + 55);
	glVertex2f(x, y - 35);
	glVertex2f(x + 50, y - 55);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex2f(x, y + 55);
	glVertex2f(x, y - 35);
	glVertex2f(x - 50, y - 55);
	glEnd();
	
	glPopMatrix();
}


void arena_warning(arena_collision ac_direction) {
	red_east = 0;
	red_north = 0;
	red_west = 0;
	red_south = 0;
	blue_east = 1;
	blue_north = 1;
	blue_west = 1;
	blue_south = 1;
	
	switch (ac_direction)
	{
	case north:
		red_north = 1;
		blue_north = 0;
		break;
	case south:
		red_south = 1;
		blue_south = 0;
		break;
	case east:
		red_east = 1;
		blue_east = 0;
		break;
	case west:
		red_west = 1;
		blue_west = 0;
		break;
	case none:
		break;
	}
}

//TODO: fix rotation (n1 priority) and fix incomplete asteroids
void draw_circle(circle_coord_array* cca, int circle_points, float rotation) {

	int err;
	while ((err = glGetError()) != GL_NO_ERROR)
		printf("display circle: %s\n", gluErrorString(err));

	glPushMatrix();
	glColor3f(1.0, 1.0, 1.0);
	glLineWidth(2.0);
	glTranslatef(cca->center.xpos, cca->center.ypos, 0);
	glRotatef(rotation, 0, 0, 1);
	glTranslatef(-cca->center.xpos, -cca->center.ypos, 0);

	glBegin(GL_LINE_LOOP);

	for (int i = 0; i < circle_points; i++) {
		glVertex2f(cca->upper[i].xpos, cca->upper[i].ypos);
	}

	for (int i = circle_points; i > 0; i--) {
		glVertex2f(cca->lower[i].xpos, cca->lower[i].ypos);
	}

	glEnd();
	glPopMatrix();
}

void draw_point(float x, float y)
{
	int err;
	while ((err = glGetError()) != GL_NO_ERROR)
		printf("display point: %s\n", gluErrorString(err));
	
	glPushMatrix();
	glPointSize(10);
	glColor3f(0, 1.0, 0);
	glBegin(GL_POINTS);
	glVertex2f(x, y);
	glEnd();
	glPopMatrix();
}

void draw_string(float x, float y, char* string) {

	int err;
	while ((err = glGetError()) != GL_NO_ERROR)
		printf("display string: %s\n", gluErrorString(err));

	glDisable(GL_TEXTURE_2D);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(-(g_screen_width / 2), g_screen_width / 2, -(g_screen_height / 2), g_screen_height / 2);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glRasterPos2i(x, y);
	void* font = GLUT_BITMAP_9_BY_15;

	for (char* c = string; *c != '\0'; c++) {
		glColor3d(1.0, 0.0, 0.0);
		glutBitmapCharacter(font, *c);
	}

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glEnable(GL_TEXTURE_2D);
}

int get_time()
{
	return (glutGet(GLUT_ELAPSED_TIME) / 1000) - time_on_death;
}

void set_reset_time()
{
	time_on_death += get_time();
}
