#include "displayController.h"
#include "shipController.h"
#include "essentials.h"
#include "asteroidController.h"

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

bool round_over = false;

void on_key_press(unsigned char key, int x, int y);
void on_key_release(unsigned char key, int x, int y);
void on_reshape(int w, int h);
void on_display();
void update_time();
void render_frame();
void on_idle();
void init_app(int* argcp, char** argv);
int main(int argc, char** argv);

void on_key_press(unsigned char key, int x, int y)
{
	//fprintf(stderr, "on_key_press()\n");
	switch (key) {
	case KEY_ESC:
		exit(EXIT_SUCCESS);
		break;
	case 'w':
		moving_forward = true;
		restart_toggle = true;
		break;
	case 'a':
		turning_left = true;
		turning_right = false;
		restart_toggle = true;
		break;
	case 'd':
		turning_left = false;
		turning_right = true;
		restart_toggle = true;
		break;
	case ' ':
		fire_bullet = true;
		restart_toggle = true;
		break;
	default:
		restart_toggle = true;
		break;
	}
}

void on_key_release(unsigned char key, int x, int y)
{
	//fprintf(stderr, "on_key_release()\n");
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
	printf("g_screen_width: %i", g_screen_width);

	fprintf(stderr, "on_reshape(%d, %d)\n", w, h);
	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-(w / 2), w / 2, -(h / 2), h / 2, -1.0, 1.0);

	//set time as seed for rand()
	srand((unsigned)time(0));

	//TODO: move this to new function so game can restart
	ship_init();
	initialise_ship_circles();
	initialise_asteroid_controller();


}

void on_display()
{
	//fprintf(stderr, "on_display()\n");
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	render_frame();

	int err;
	while ((err = glGetError()) != GL_NO_ERROR)
		printf("error: %s\n", gluErrorString(err));

	glutSwapBuffers();
}

void update_game_state()
{

}


void render_frame()
{
	draw_arena(g_screen_width, g_screen_height);
	if (!game_over) {
		ship_controller();
		asteroid_controller();
		particle_controller();
		bullet_controller();
	}
	else {
		//update_time();
		draw_string(-20, 0, "Game Over");
		if (restart_toggle)
		{
			restart();
		}
	}
}

void initialise_game()
{

}



void on_idle()
{
	
	//float dt = cur_time - g_last_time;
	//update_game_state(g_game_objects, dt);
	//asteroid_movement(&active_asteroids[0]);
	//g_last_time = cur_time;
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

	//g_last_time = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
	// *** DO NOT call game_object_init() here. WHY?
}

int main(int argc, char** argv)
{
	init_app(&argc, argv);
	glutMainLoop();
	return(EXIT_SUCCESS);
}
