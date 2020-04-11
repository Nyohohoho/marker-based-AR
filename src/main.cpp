#include "parameters.h"
#include "draw_graphics.h"
#include "marker_detection.h"
#include "graphics_utility.h"

#include <cstdio>
#include <cstdlib>

#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>

// Include GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

int main() {
	// Use my PC's internal camera
	// Open the internal camera
	cv::VideoCapture internal_camera(0);
	// Set width as 1280 pixels
	internal_camera.set(3, 1280);
	// Set height as 720 pixels
	internal_camera.set(4, 720);

	// Current frame from the internal camera
	cv::Mat current_frame;

	GLFWwindow* window = nullptr;
	initializeGL(window);

	// Load the shaders for drawing background
	GLuint background_shader_id = loadShaders(
		"background_vertex_shader.vert",
		"background_fragment_shader.frag");

	// Load the shaders for drawing the bunny with specular shading
	GLuint shading_shader_id = loadShaders(
		"shading_vertex_shader.vert",
		"shading_fragment_shader.frag");

	/** ply file only contains the information about vertices
	  * so the following code only create a bunny with color of red-blue
	GLuint color_shader_id = loadShaders(
		"color_vertex_shader.vert",
		"color_fragment_shader.frag");
	std::vector<glm::vec3> color_bunny_vertices;
	oadPly("Model/bun_zipper_res4.ply", color_bunny_vertices);
	*/

	std::vector<glm::vec3> shading_bunny_vertices, shading_bunny_normals;
	loadObj("Model/bun_zipper.obj",
		shading_bunny_vertices, shading_bunny_normals);

	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (internal_camera.read(current_frame)) {
			// Convert BGR to RGB
			cv::cvtColor(current_frame, current_frame, cv::COLOR_BGR2RGB);

			// Record the poses of the markers
			std::vector<cv::Mat> all_marker_poses;
			detectMarkersAndEstimatePose(current_frame, all_marker_poses);
			// Switch the image origin from top-left to bottom-left image
			// in order to draw in OpenGL
			cv::flip(current_frame, current_frame, 0);

			// Draw the current frame as background
			drawBackground(current_frame, background_shader_id);
			glClear(GL_DEPTH_BUFFER_BIT);

			glm::mat4 projection;
			buildProjection(current_frame, projection);
			// Rotate around x-axis
			glm::vec3 rotation_axis(1.0f, 0.0f, 0.0f);
			// Rotation is to make the bunny sit on the marker
			// If translation is not applied,
			// the bunny will sit on top of the marker
			// Scaling is to shrink the size of the bunny
			glm::mat4 model =
				glm::rotate(glm::mat4(),
					glm::radians(90.0f), rotation_axis) *
				glm::translate(glm::mat4(),
					glm::vec3(0.0f, -MARKER_LENGTH, 0.0f)) *
				glm::scale(glm::mat4(),
					glm::vec3(0.5f, 0.5f, 0.5f));
			size_t num_of_marker = all_marker_poses.size();
			for (size_t i = 0; i < num_of_marker; i++) {
				cv::Mat current_marker_pose = all_marker_poses[i];
				glm::mat4 view = glm::make_mat4(
					reinterpret_cast<GLfloat*>(
						current_marker_pose.data));

				/** This is the code for drawing color bunny
				drawColorBunny(
					color_bunny_vertices,
					model, view, projection,
					color_shader_id);
				*/

				drawShadingBunny(
					shading_bunny_vertices, shading_bunny_normals,
					model, view, projection,
					shading_shader_id);
			}

			glfwSwapBuffers(window);
		}
	}

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}
