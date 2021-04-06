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

float red, green = 0;
float blue = 1;
int g_screen_width = 0;
int g_screen_height = 0;
int time_on_death = 0;


void draw_arena(int screen_width, int screen_height) {
	glPushMatrix();
	glColor3f(red, green, blue);
	glLineWidth(100);

	glBegin(GL_LINE_LOOP);

	//TODO: use g_screen_width etc instead
	glVertex2f(-(screen_width / 2), -(screen_height / 2));
	glVertex2f(screen_width / 2, -(screen_height / 2));
	glVertex2f(screen_width / 2, (screen_height / 2));
	glVertex2f(-(screen_width / 2), screen_height / 2);

	glEnd();
	glPopMatrix();
}

void draw_ship(float x, float y, float velocity, float direction) {
	glPushMatrix();
	glTranslatef(x, y, velocity);
	glRotatef(-direction, 0, 0, 1);
	glBegin(GL_TRIANGLE_STRIP);
	glVertex2f(0, 110);
	glVertex2f(0, 0);
	glVertex2f(50, -20);
	glEnd();

	glBegin(GL_TRIANGLE_STRIP);
	glVertex2f(0, 110);
	glVertex2f(0, 0);
	glVertex2f(-50, -20);
	glEnd();
	glPopMatrix();
}


void arena_warning(bool on) {
	if (on)
	{
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

void draw_circle(circle_coord_array* cca, int circle_points) {

	glPushMatrix();
	glColor3f(1.0, 1.0, 1.0);
	glLineWidth(2.0);
	//glRotatef(10, 0, 0, 1);
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
	glPushMatrix();
	glPointSize(10);
	glColor3f(0, 1.0, 0);
	glBegin(GL_POINTS);
	glVertex2f(x, y);
	glEnd();
	glPopMatrix();
}

void draw_string(float x, float y, char* string) {
	
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
	time_on_death = get_time() + time_on_death;
}
