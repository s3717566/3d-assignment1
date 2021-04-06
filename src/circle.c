#include "circle.h"

void move_circle(circle_coord_array* cca, float DistTravelledX, float DistTravelledY, int circle_points) {

	/*for (int i = 0; i < circle_points; i++) {
		cca->upper[i].xpos = cca->upper[i].xpos - DistTravelledX;
		cca->upper[i].ypos = cca->upper[i].ypos - DistTravelledY;
	}

	for (int i = circle_points; i > 0; i--) {
		cca->lower[i].xpos = cca->lower[i].xpos - DistTravelledX;
		cca->lower[i].ypos = cca->lower[i].ypos - DistTravelledY;
	}*/
	cca->center.xpos += DistTravelledX;
	cca->center.ypos += DistTravelledY;
}

bool check_collision(float x1, float y1, float x2, float y2, float radius1, float radius2) {
	return fabs((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2)) <= (radius1 + radius2) * (radius1 + radius2);
}


void initialise_circle(float radius, circle_coord_array* cca, float initialX, float initialY, int circle_points, bool make_random) {

	//printf("Initialised circle shape");

	float x, y;
	float randomnessX = 0;
	float randomnessY = 0;
	//top semicircle
	for (int i = 0; i < circle_points; i++) {
		if (make_random)
		{
			randomnessX = 10.0 - (rand() % 100 * 0.1);
			randomnessY = 10.0 - (rand() % 100 * 0.1);
		}

		x = ((i / (float)circle_points - 0.5) * 2.0) * radius + randomnessX;
		y = sqrt(radius * radius - x * x) + randomnessY;
		cca->upper[i].xpos = x + initialX;
		cca->upper[i].ypos = y + initialY;
	}

	//bottom semicircle
	for (int i = circle_points; i > 0; i--) {
		if (make_random)
		{
			randomnessX = 5.0 - (rand() % 100 * 0.1);
			randomnessY = 5.0 - (rand() % 100 * 0.1);
		}
		x = (i / (float)circle_points - 0.5) * 2.0 * radius + randomnessX;
		y = -sqrt(radius * radius - x * x) + randomnessY;
		cca->lower[i].xpos = x + initialX;
		cca->lower[i].ypos = y + initialY;
	}

	cca->center.xpos = initialX;
	cca->center.ypos = initialY;
}

void rotate_circle(circle_coord_array* cca, float rotation, int circle_points)
{
	float firstXupper = cca->upper[0].xpos;
	float firstYupper = cca->upper[0].ypos;
	
	for (int i = 0; i < circle_points - 1; i++) {
		cca->upper[i].xpos = cca->upper[i + 1].xpos;
		cca->upper[i].ypos = cca->upper[i + 1].ypos;
	}

	cca->upper[circle_points - 1].xpos = firstXupper;
	cca->upper[circle_points - 1].ypos = firstYupper;

	float firstXlower = cca->lower[circle_points - 1].xpos;
	float firstYlower = cca->lower[circle_points - 1].ypos;
	
	for (int i = circle_points; i > 0; i--) {
		cca->lower[i].xpos = cca->lower[i - 1].xpos;
		cca->lower[i].ypos = cca->lower[i - 1].ypos;
	}

	cca->lower[0].xpos = firstXlower;
	cca->lower[0].ypos = firstYlower;
}