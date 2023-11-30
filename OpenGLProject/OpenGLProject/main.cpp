#include <freeglut.h>
#include <Windows.h>

#include <cstdint>
#include <algorithm>
#include <iomanip>
#include <iostream>

// Definalbe variables--
float BackgroundColour[3] = { 0.227, 0.53, 0.81 };

// System variables (don't change)
bool Running = true;


// Functions
void Initialise() {
	glutSetColor(1.0, 1.0, 0.0, 0.0);
}

void ChangeSize(int w, int h) {

	float AspectRatio = w * 1.0 / h;

	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();
	glViewport(0, 0, w, h);
	gluPerspective(45, AspectRatio, 1, 1000);

	glMatrixMode(GL_MODELVIEW);

}

void RenderScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBegin(GL_TRIANGLES);
		glVertex3f(-2, -2, -5.0);
		glVertex3f(2, 0.0, -5.0);
		glVertex3f(0.0, 2, -5.0);
	glEnd();

	glutSwapBuffers();
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	std::cout << GLUT_API_VERSION;

	glutInitWindowPosition(200, 200);
	glutInitWindowSize(1280,720);

	glutCreateWindow("OpenGL Demonstration");

	glClearColor(BackgroundColour[0], BackgroundColour[1], BackgroundColour[2],1);

	glutDisplayFunc(RenderScene);
	glutReshapeFunc(ChangeSize);

	glutMainLoop();

	return 1;
}

