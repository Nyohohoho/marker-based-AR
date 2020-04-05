#include "parameters.h"
#include "draw_graphics.h"
#include "graphics_utility.h"
#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>

#include <GL/glut.h>

int main(int argc, char** argv) {
	/*
	initialzeCamera();

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	// Set window as 1280x720
	glutInitWindowSize(1280, 720);
	glutCreateWindow("Marker Based Augmented Reality");
	glutDisplayFunc(&display);
	glutTimerFunc(0, timer, 0);

	glutMainLoop();
	*/

	std::vector<float> output_vertices;
	std::vector<int> output_face_indices;
	loadPly("Model/bun_zipper.ply", output_vertices, output_face_indices);

	return 0;
}