/*
* Author : Jacob Winters (jwinterscs@gmail.com)
* Project : CS-330 Module Eight: Coding Collisions
* Date : 04/19/22
*/

//-------------------------------------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------------------------------------
#include <GLFW\glfw3.h>				// Window creation / management, and keyboard / mouse input library
#include "linmath.h"				// Linear math library
#include <stdlib.h>                 // Standard general utilities library
#include <stdio.h>					// C library to perform input / output operations
#include <conio.h>					// Console input / output
#include <iostream>					// Standard input / output streams library
#include <vector>					// Vector sequence containers
#include <windows.h>				// WIN32 API
#include <time.h>					// C Time Library

// Standard namespace
using namespace std;

const float DEG2RAD = 3.14159 / 180;

enum BRICKTYPE { REFLECTIVE, DESTRUCTABLE, PADDLE};
enum ONOFF { ON, OFF };

class Brick
{
public:
	float red, green, blue;
	float x, y, width;
	double halfside;
	double top;
	double bottom;
	double left;
	double right;
	BRICKTYPE brick_type;
	ONOFF onoff;

	Brick(BRICKTYPE bt, float xx, float yy, float ww, float rr, float gg, float bb)
	{
		brick_type = bt; x = xx; y = yy, width = ww; red = rr, green = gg, blue = bb;
		halfside = width / 2;
		top = y + halfside;
		bottom = y - halfside;
		left = x - halfside;
		right = x + halfside;
		onoff = ON;
	};

	void drawBrick()
	{
		if (onoff == ON)
		{
			double halfside = width / 2;

			glColor3d(red, green, blue);
			glBegin(GL_POLYGON);
			if (brick_type == PADDLE) {
				right = x + halfside;
				left = x - halfside;
				glVertex2d(right, y);
				glVertex2d(right, (y - 0.1));
				glVertex2d(left, (y - 0.1));
				glVertex2d(left, y);
			}
			else {
				glVertex2d(x + halfside, y + halfside);
				glVertex2d(x + halfside, y - halfside);
				glVertex2d(x - halfside, y - halfside);
				glVertex2d(x - halfside, y + halfside);
			}

			glEnd();
		}
	}
};


class Circle
{
public:
	float red, green, blue;
	float radius;
	float x;
	float y;
	float top;
	float bottom;
	float left;
	float right;
	float speed = 0.01;
	int direction; // 1=Up 2=Right 3=Down 4=Left 5=Up right 6=Up left 7=Down right 8=Down left

	void physics(double edge);

	Circle(double xx, double yy, int dir, float rad, float r, float g, float b)
	{
		x = xx;
		y = yy;
		top = y + radius;
		bottom = y - radius;
		left = x - radius;
		right = x + radius;
		red = r;
		green = g;
		blue = b;
		radius = rad;
		direction = dir;
	}

	void CheckCollision(Brick* brk)
	{
		if (brk->brick_type == REFLECTIVE)
		{

			// Top Collision
			if ((x >= brk->left && x < brk->right) && ((y - radius) >= brk->bottom && (y - radius) < brk->top))
			{
				physics("above");
				brk->brick_type = DESTRUCTABLE;
				brk->blue = colRand();
				brk->green = colRand();
				brk->red = colRand();
			}
			// Bottom Collision
			else if ((x > brk->left && x <= brk->right) && ((y + radius) > brk->bottom && (y + radius) <= brk->top))
			{
				physics("below");
				brk->brick_type = DESTRUCTABLE;
				brk->blue = colRand();
				brk->green = colRand();
				brk->red = colRand();
			}
			// Left Collision
			else if (((x + radius) > brk->left && (x - radius) <= brk->right) && (y >= brk->bottom && y < brk->top))
			{
				physics("leftside");
				brk->brick_type = DESTRUCTABLE;
				brk->blue = colRand();
				brk->green = colRand();
				brk->red = colRand();
			}
			// Right Collision
			else if (((x - radius) >= brk->left && (x - radius) < brk->right) && (y > brk->bottom && y <= brk->top))
			{
				physics("rightside");
				brk->brick_type = DESTRUCTABLE;
				brk->blue = colRand();
				brk->green = colRand();
				brk->red = colRand();
			}
		}

		else if (brk->brick_type == DESTRUCTABLE)
		{
			if (brk->onoff == ON)
			{
				// Top Collision
				if ((x >= brk->left && x < brk->right) && ((y - radius) >= brk->bottom && (y - radius) < brk->top))
				{
					physics("above");
					brk->onoff = OFF;
				}
				// Bottom Collision
				else if ((x > brk->left && x <= brk->right) && ((y + radius) > brk->bottom && (y + radius) <= brk->top))
				{
					physics("below");
					brk->onoff = OFF;
				}
				// Left Collision
				else if (((x + radius) > brk->left && (x - radius) <= brk->right) && (y >= brk->bottom && y < brk->top))
				{
					physics("leftside");
					brk->onoff = OFF;
				}
				// Right Collision
				else if (((x - radius) >= brk->left && (x - radius) < brk->right) && (y > brk->bottom && y <= brk->top))
				{
					physics("rightside");
					brk->onoff = OFF;
				}
			}
		}
		else if (brk->brick_type == PADDLE) 
		{
			if ((x >= brk->left && x <= brk->right) && ((y - radius) >= brk->bottom && (y - radius) < brk->y)) {
				if (x >= brk->left && x < (brk->left + (brk->width / 3))) 
				{
					direction = 8;
				}
				else if (x >= (brk->left + (brk->width / 3)) && x <= (brk->left + ((brk->width / 3) * 2)))
				{
					direction = 3;
				}
				else if (x > (brk->left + ((brk->width / 3) * 2)) && x <= brk->right)
				{
					direction = 7;
				}
			}
		}
	}

	float colRand()
	{
		float rn = rand() % 100 + 1; // Random number between 1-100
		rn = rn / 100; // Round number from 0.01 to 1.0
		return rn; // Return rounded number
	}

	bool MoveOneStep()
	{
		bool alive = true;

		if (direction == 1 || direction == 5 || direction == 6)
		{
			if (y > -1 + radius)
			{
				y -= speed;
				bottom = y - radius;
				top = y + radius;
			}
			else
			{
				alive = false;
			}
		}

		if (direction == 2 || direction == 5 || direction == 7)
		{
			if (x < 1 - radius)
			{
				x += speed;
				right = x + radius;
				left = x - radius;
			}
			else
			{
				physics("leftside");
			}
		}

		if (direction == 3 || direction == 7 || direction == 8)
		{
			if (y < 1 - radius) {
				y += speed;
				bottom = y - radius;
				top = y + radius;
			}
			else
			{
				physics("below");
			}
		}

		if (direction == 4 || direction == 6 || direction == 8)
		{
			if (x > -1 + radius) {
				x -= speed;
				right = x + radius;
				left = x - radius;
			}
			else
			{
				physics("rightside");
			}
		}
		return alive;
	}

	void DrawCircle()
	{
		glColor3f(red, green, blue);
		glBegin(GL_POLYGON);
		for (int i = 0; i < 360; i++) {
			float degInRad = i * DEG2RAD;
			glVertex2f((cos(degInRad) * radius) + x, (sin(degInRad) * radius) + y);
		}
		glEnd();
	}
	// Physics Movement
	void physics(string pos)
	{
		if (pos == "above") {
			if (direction == 1) { direction = 3; }
			else if (direction == 5) { direction = 7; }
			else { direction = 8; }
		}
		else if (pos == "leftside") {
			if (direction == 2) { direction = 4; }
			else if (direction == 5) { direction = 6; }
			else { direction = 8; }
		}
		else if (pos == "below") {
			if (direction == 3) { direction = 1; }
			else if (direction == 7) { direction = 5; }
			else { direction = 6; }
		}
		else if (pos == "rightside") {
			if (direction == 4) { direction = 2; }
			else if (direction == 6) { direction = 5; }
			else { direction = 7; }
		}
	}

	void CheckCircleCollision(Circle* circ)
	{
		// Top Collision
		if ((x >= circ->left && x < circ->right) && ((y - radius) >= circ->bottom && (y - radius) < circ->top))
		{
			physics("above");
			red = rand() % 10 * .1;
			blue = rand() % 10 * .1;
			green = rand() % 10 * .1;
		}
		// Bottom Collision
		else if ((x > circ->left && x <= circ->right) && ((y + radius) > circ->bottom && (y + radius) <= circ->top))
		{
			physics("below");
			red = rand() % 10 * .1;
			blue = rand() % 10 * .1;
			green = rand() % 10 * .1;
		}
		// Left Collision
		else if (((x + radius) > circ->left && (x - radius) <= circ->right) && (y >= circ->bottom && y < circ->top))
		{
			physics("leftside");
			red = rand() % 10 * .1;
			blue = rand() % 10 * .1;
			green = rand() % 10 * .1;
		}
		// Right Collision
		else if (((x - radius) >= circ->left && (x - radius) < circ->right) && (y > circ->bottom && y <= circ->top))
		{
			physics("rightside");
			red = rand() % 10 * .1;
			blue = rand() % 10 * .1;
			green = rand() % 10 * .1;
		}
	}
};

vector<Circle> world;

// Draw Paddle
Brick paddle(PADDLE, 0.0, -0.8, 0.50, 0, 0, 1);

void processInput(GLFWwindow* window, Brick* paddle);

int main(void) {
	srand(time(NULL));

	if (!glfwInit()) {
		exit(EXIT_FAILURE);
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	GLFWwindow* window = glfwCreateWindow(480, 480, "OpenGL Animation", NULL, NULL);
	if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	// Brick Layout
	Brick brick(REFLECTIVE, 0.5, -0.15, 0.2, 0, 1, 1);
	Brick brick2(REFLECTIVE, -0.5, 0.35, 0.2, 0, 1, 1);
	Brick brick3(REFLECTIVE, -0.5, -0.15, 0.2, 0, 1, 1);
	Brick brick4(REFLECTIVE, 0, -0.40, 0.2, 0, 1, 1);
	Brick brick5(REFLECTIVE, -0.5, -0.40, 0.2, 0, 1, 1);
	Brick brick6(REFLECTIVE, 0.5, -0.40, 0.2, 0, 1, 1);
	Brick brick7(REFLECTIVE, 0.5, 0.35, 0.2, 0, 1, 1);
	Brick brick8(REFLECTIVE, 0.25, -0.40, 0.2, 0, 1, 1);
	Brick brick9(REFLECTIVE, -0.25, -0.40, 0.2, 0, 1, 1);

	Brick brick10(REFLECTIVE, -0.75, -0.40, 0.2, 1, 1, 1);
	Brick brick11(REFLECTIVE, -0.75, 0.85, 0.2, 1, 1, 1);
	Brick brick12(REFLECTIVE, -0.75, 0.10, 0.2, 1, 1, 1);
	Brick brick13(REFLECTIVE, -0.75, 0.35, 0.2, 1, 1, 1);
	Brick brick14(REFLECTIVE, -0.75, -0.15, 0.2, 1, 1, 1);
	Brick brick15(REFLECTIVE, -0.75, 0.60, 0.2, 1, 1, 1);

	Brick brick16(REFLECTIVE, 0.75, -0.40, 0.2, 1, 1, 1);
	Brick brick17(REFLECTIVE, 0.75, 0.60, 0.2, 1, 1, 1);
	Brick brick18(REFLECTIVE, 0.75, 0.10, 0.2, 1, 1, 1);
	Brick brick19(REFLECTIVE, 0.75, 0.35, 0.2, 1, 1, 1);
	Brick brick20(REFLECTIVE, 0.75, -0.15, 0.2, 1, 1, 1);
	Brick brick21(REFLECTIVE, 0.75, 0.85, 0.2, 1, 1, 1);

	Brick brick22(REFLECTIVE, 0.50, 0.85, 0.2, 1, 1, 1);
	Brick brick23(REFLECTIVE, 0.50, 0.60, 0.2, 1, 1, 1);
	Brick brick24(REFLECTIVE, 0.50, 0.10, 0.2, 1, 1, 1);
	Brick brick25(REFLECTIVE, 0.25, 0.35, 0.2, 1, 1, 1);
	Brick brick26(REFLECTIVE, -0.25, 0.60, 0.2, 1, 1, 1);
	Brick brick27(REFLECTIVE, 0.25, 0.85, 0.2, 1, 1, 1);

	Brick brick28(REFLECTIVE, 0, 0.85, 0.2, 1, 1, 1);
	Brick brick29(REFLECTIVE, 0, 0.60, 0.2, 1, 1, 1);
	Brick brick30(REFLECTIVE, -0.25, 0.10, 0.2, 1, 1, 1);
	Brick brick31(REFLECTIVE, -0.50, 0.10, 0.2, 1, 1, 1);
	Brick brick32(REFLECTIVE, 0.25, -0.15, 0.2, 1, 1, 1);
	Brick brick33(REFLECTIVE, -0.50, 0.85, 0.2, 1, 1, 1);

	Brick brick34(REFLECTIVE, -0.25, 0.85, 0.2, 1, 1, 1);
	Brick brick35(REFLECTIVE, -0.50, 0.60, 0.2, 1, 1, 1);
	Brick brick36(REFLECTIVE, 0, 0.10, 0.2, 1, 1, 1);
	Brick brick37(REFLECTIVE, 0, -0.15, 0.2, 1, 1, 1);

	Brick brick38(REFLECTIVE, 0.25, 0.10, 0.2, 1, 1, 1);
	Brick brick39(REFLECTIVE, 0.25, 0.60, 0.2, 1, 1, 1);
	Brick brick40(REFLECTIVE, -0.25, 0.35, 0.2, 1, 1, 1);
	Brick brick41(REFLECTIVE, 0, 0.35, 0.2, 1, 1, 1);
	Brick brick42(REFLECTIVE, -0.25, -0.15, 0.2, 1, 1, 1);

	while (!glfwWindowShouldClose(window)) {
		// Setup View
		float ratio;
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		ratio = width / (float)height;
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT);

		processInput(window, &paddle);

		// Check collisions and draw bricks
		for (int i = 0; i < world.size(); i++)
		{
			world[i].CheckCollision(&brick);
			world[i].CheckCollision(&brick2);
			world[i].CheckCollision(&brick3);
			world[i].CheckCollision(&brick4);
			world[i].CheckCollision(&brick5);
			world[i].CheckCollision(&brick6);
			world[i].CheckCollision(&brick7);
			world[i].CheckCollision(&brick8);
			world[i].CheckCollision(&brick9);
			world[i].CheckCollision(&brick10);
			world[i].CheckCollision(&brick11);
			world[i].CheckCollision(&brick12);
			world[i].CheckCollision(&brick13);
			world[i].CheckCollision(&brick14);
			world[i].CheckCollision(&brick15);
			world[i].CheckCollision(&brick16);
			world[i].CheckCollision(&brick17);
			world[i].CheckCollision(&brick18);
			world[i].CheckCollision(&brick19);
			world[i].CheckCollision(&brick20);
			world[i].CheckCollision(&brick21);
			world[i].CheckCollision(&brick22);
			world[i].CheckCollision(&brick23);
			world[i].CheckCollision(&brick24);
			world[i].CheckCollision(&brick25);
			world[i].CheckCollision(&brick26);
			world[i].CheckCollision(&brick27);
			world[i].CheckCollision(&brick28);
			world[i].CheckCollision(&brick29);
			world[i].CheckCollision(&brick30);
			world[i].CheckCollision(&brick31);
			world[i].CheckCollision(&brick32);
			world[i].CheckCollision(&brick33);
			world[i].CheckCollision(&brick34);
			world[i].CheckCollision(&brick35);
			world[i].CheckCollision(&brick36);
			world[i].CheckCollision(&brick37);
			world[i].CheckCollision(&brick38);
			world[i].CheckCollision(&brick39);
			world[i].CheckCollision(&brick40);
			world[i].CheckCollision(&brick41);
			world[i].CheckCollision(&brick42);
			world[i].CheckCollision(&paddle);
			world[i].DrawCircle();
			world[i].MoveOneStep();
			// Circle Collision Loop
			for (int j = i + 1; j < world.size(); j++)
			{
				world[j].CheckCircleCollision(&world[i]);
				world[i].CheckCircleCollision(&world[j]);
			}
		}

		brick.drawBrick();
		brick2.drawBrick();
		brick3.drawBrick();
		brick4.drawBrick();
		brick5.drawBrick();
		brick6.drawBrick();
		brick7.drawBrick();
		brick8.drawBrick();
		brick9.drawBrick();
		brick10.drawBrick();
		brick11.drawBrick();
		brick12.drawBrick();
		brick13.drawBrick();
		brick14.drawBrick();
		brick15.drawBrick();
		brick16.drawBrick();
		brick17.drawBrick();
		brick18.drawBrick();
		brick19.drawBrick();
		brick20.drawBrick();
		brick21.drawBrick();
		brick22.drawBrick();
		brick23.drawBrick();
		brick24.drawBrick();
		brick25.drawBrick();
		brick26.drawBrick();
		brick27.drawBrick();
		brick28.drawBrick();
		brick29.drawBrick();
		brick30.drawBrick();
		brick31.drawBrick();
		brick32.drawBrick();
		brick33.drawBrick();
		brick34.drawBrick();
		brick35.drawBrick();
		brick36.drawBrick();
		brick37.drawBrick();
		brick38.drawBrick();
		brick39.drawBrick();
		brick40.drawBrick();
		brick41.drawBrick();
		brick42.drawBrick();
		paddle.drawBrick();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate;
	exit(EXIT_SUCCESS);
}


void processInput(GLFWwindow* window, Brick* paddle)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		if (world.size() < 2)
		{
			double r, g, b;
			r = rand() / 10000;
			g = rand() / 10000;
			b = rand() / 10000;
			Circle B(paddle->x, -0.75, 3, 0.05, 1.0, 1.0, 1.0);
			world.push_back(B);
		}
	}
	// Paddle Controls
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		if (paddle->left > -1.0) {
			paddle->x -= .02;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		if (paddle->right < 1.0) {
			paddle->x += .02;
		}
	}
}