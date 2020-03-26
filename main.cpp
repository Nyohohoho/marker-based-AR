#include <vector>
#include <stdlib.h>

#include <GL/glut.h>

#include <opencv2/opencv.hpp>

#include "geometric_types.h"
#include "marker.h"
#include "marker_detection.h"

static GLint image_width;
static GLint image_height;
static GLubyte* pixeldata;

// Detect markers and estimate their poses
static MarkerDetection marker_detection;

// Use my PC's internal camera
static cv::VideoCapture internal_camera;
// Current frame from the internal camera
static cv::Mat current_frame;

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw the current frame
	glDrawPixels(image_width, image_height,
		GL_BGR_EXT, GL_UNSIGNED_BYTE, pixeldata);

	/*--------------------Draw cubes on detected markers---------------------*/
	glPushMatrix();

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(marker_detection.getProjectionMatrix());
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// Make line thicker
	glLineWidth(3.0f);
	for (size_t i = 0; i < marker_detection.getDetectedMarkers().size(); i++) {
		glPushMatrix();

		const Transformation& transformation =
			marker_detection.getDetectedMarkers()[i].transformation;
		Matrix44 gl_matrix = transformation.getMat44();

		// Load the camera pose
		glLoadMatrixf(reinterpret_cast<const GLfloat*>(gl_matrix.data));

		glColor3f(1.0f, 0, 0);
		glutWireCube(0.75);

		glPopMatrix();
	}
	glFlush();

	glPopMatrix();
	/*---------------------------------End-----------------------------------*/

	glutSwapBuffers();
}

// Called by OpenGl glutTimerFunc
// for marker detection and camera pose estimation
void timer(int) {
	glutPostRedisplay();
	glutTimerFunc(10, timer, 0);

	if (internal_camera.read(current_frame)) {
		marker_detection.setImage(current_frame);
		marker_detection.detectAndEstimate();

		// Switch between top-left and bottom-left image origin
		// in order to draw in OpenGL
		cv::flip(current_frame, current_frame, 0);

		image_width = current_frame.cols;
		image_height = current_frame.rows;
		pixeldata = reinterpret_cast<GLubyte*>(current_frame.data);
	}
}

int main(int argc, char** argv) {
	// Open the internal camera
	internal_camera.open(0);
	// Set width as 1280 pixels
	internal_camera.set(3, 1280);
	// Set height as 720 pixels
	internal_camera.set(4, 720);
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	// Set window as 1280x720
	glutInitWindowSize(1280, 720);
	glutCreateWindow("Marker Based Augmented Reality");
	glutDisplayFunc(&display);
	glutTimerFunc(0, timer, 0);

	glutMainLoop();

	return 0;
}