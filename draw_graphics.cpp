#include "draw_graphics.h"
#include "parameters.h"
#include "marker_detection.h"

#include <GL/glut.h>

// Initialzie my PC's internal camera
void initialzeCamera() {
	// Open the internal camera
	internal_camera.open(0);
	// Set width as 1280 pixels
	internal_camera.set(3, 1280);
	// Set height as 720 pixels
	internal_camera.set(4, 720);

	image_width = 1280;
	image_height = 720;
	pixeldata = nullptr;
}

// Build the projection matrix for rendering on current frame
void buildProjectionMatrix() {
	// Near clipping distance
	float clipping_near = 0.01f;
	// Far clipping distance
	float clipping_far = 100.0f;

	// (Camera parameters) Focal length in x axis
	float focal_length_x = 9.4721585489646418e+02f;
	// (Camera parameters) Focal length in y axis
	float focal_length_y = 9.4984601436177400e+02f;
	// (Camera parameters) Principle point in x axis
	float principle_point_x = 6.5256929713596503e+02f;
	// (Camera parameters) Principle point in y axis
	float principle_point_y = 3.6638013113209280e+02f;

	projection_matrix[0] = -2.0f * focal_length_x / image_width;
	projection_matrix[1] = 0.0f;
	projection_matrix[2] = 0.0f;
	projection_matrix[3] = 0.0f;

	projection_matrix[4] = 0.0f;
	projection_matrix[5] = 2.0f * focal_length_y / image_height;
	projection_matrix[6] = 0.0f;
	projection_matrix[7] = 0.0f;

	projection_matrix[8] = 2.0f * principle_point_x / image_width - 1.0;
	projection_matrix[9] = 2.0f * principle_point_y / image_height - 1.0;
	projection_matrix[10] =
		-(clipping_far + clipping_near) / (clipping_far - clipping_near);
	projection_matrix[11] = -1.0f;

	projection_matrix[12] = 0.0f;
	projection_matrix[13] = 0.0f;
	projection_matrix[14] =
		-2.0f * clipping_far * clipping_near / (clipping_far - clipping_near);
	projection_matrix[15] = 0.0f;
}

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw the current frame
	glDrawPixels(image_width, image_height,
		GL_BGR_EXT, GL_UNSIGNED_BYTE, pixeldata);

	/*--------------------Draw cubes on detected markers---------------------*/
	glPushMatrix();

	glMatrixMode(GL_PROJECTION);
	buildProjectionMatrix();
	glLoadMatrixf(projection_matrix);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	// Make line thicker
	glLineWidth(3.0f);

	size_t num_of_marker = all_marker_poses.size();
	for (size_t i = 0; i < num_of_marker; i++) {
		glPushMatrix();

		cv::Mat_<float> current_marker_pose = all_marker_poses[i].clone();
		// Load the camera pose
		glLoadMatrixf(
			reinterpret_cast<const GLfloat*>(current_marker_pose.data));

		glColor3f(1.0f, 0.0f, 0.0f); // Red X-axis
		glBegin(GL_LINES);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(3.0f, 0.0f, 0.0f);
		glEnd();
		glColor3f(0.0, 1.0, 0.0); // Green Y-axis
		glBegin(GL_LINES);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 3.0f, 0.0f);
		glEnd();
		glColor3f(0.0, 0.0, 1.0); // Blue Z-axis
		glBegin(GL_LINES);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 0.0f, 3.0f);
		glEnd();

		glColor3f(0.5f, 0.5f, 0.5f);
		glutSolidCube(1.0f);
		
		glPopMatrix();
	}

	glFlush();

	glPopMatrix();
	/*---------------------------------End-----------------------------------*/

	glutSwapBuffers();
}

// Used for marker detection and camera pose estimation
// Called by OpenGl glutTimerFunc
void timer(int) {
	glutPostRedisplay();
	glutTimerFunc(10, timer, 0);

	if (internal_camera.read(current_frame)) {
		detectMarkersAndEstimatePose(current_frame, all_marker_poses);

		// Switch between top-left and bottom-left image origin
		// in order to draw in OpenGL
		cv::flip(current_frame, current_frame, 0);

		image_width = current_frame.cols;
		image_height = current_frame.rows;
		pixeldata = reinterpret_cast<GLubyte*>(current_frame.data);
	}
}
