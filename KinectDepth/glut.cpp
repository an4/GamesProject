#include "glut.h"
#include "main.h"

void draw() {
   drawKinectData();
   glutSwapBuffers();
}

void execute() {
    glutMainLoop();
}

void keyPressed(unsigned char key, int x, int y)
{
	if (key == 'a') {
		initiateDump();
	}
}

bool init(int argc, char* argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(width,height);
    glutCreateWindow("Kinect Depth Map");
    glutDisplayFunc(draw);
    glutIdleFunc(draw);
	glutKeyboardFunc(keyPressed);
    return true;
}
