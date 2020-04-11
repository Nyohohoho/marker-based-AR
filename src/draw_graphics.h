#pragma once

#ifndef DRAW_GRAPHICS
#define DRAW_GRAPHICS

#include <vector>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <opencv2/opencv.hpp>

// Initialize OpenGL
bool initializeGL(GLFWwindow*& window);

// Build the projection matrix for rendering on current frame
void buildProjection(
	const cv::Mat& input_frame,
	glm::mat4& output_projection);

// Turn a cv::Mat into a texture, and return the texture ID as a GLuint
// It's for OpenGL to draw the frame captured by camera
GLuint mat2texture(const cv::Mat& input_image);

// Draw the frame captured by my PC's internal camera
void drawBackground(const cv::Mat& input_image, const GLuint& program_id);

// Draw the bunny in ply file with random colors
void drawColorBunny(
	const std::vector<glm::vec3>& vertices,
	const glm::mat4& model_matrix,
	const glm::mat4& view_matrix,
	const glm::mat4& projection_matrix,
	const GLuint& program_id);

// Draw the bunny in obj file with specular shading
void drawShadingBunny(
	const std::vector<glm::vec3>& vertices,
	const std::vector<glm::vec3>& normals,
	const glm::mat4& model_matrix,
	const glm::mat4& view_matrix,
	const glm::mat4& projection_matrix,
	const GLuint& program_id);

#endif // !DRAW_GRAPHICS
