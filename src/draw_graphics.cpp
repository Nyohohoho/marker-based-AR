#include "draw_graphics.h"
#include "graphics_utility.h"

#include <cstdio>
#include <cstdlib>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Initialize GLFW, GLEW
bool initializeGL(GLFWwindow*& window) {
	// Initialise GLFW
	if (!glfwInit()) {
		std::fprintf(stderr, "Failed to initialize GLFW.\n");
		std::getchar();
		// Fail
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(1280, 720,
		"Marker Based Augmented Reality", NULL, NULL);
	if (window == NULL) {
		std::fprintf(stderr, "Failed to open GLFW window.\n");
		std::getchar();
		glfwTerminate();
		// Fail
		return false;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		std::fprintf(stderr, "Failed to initialize GLEW.\n");
		std::getchar();
		glfwTerminate();
		// Fail
		return false;
	}

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);
	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Successfully initialized
	return true;
}

// Build the projection matrix for rendering on current frame
void buildProjection(
	const cv::Mat& input_frame,
	glm::mat4& output_projection) {
	GLint frame_width = input_frame.cols;
	GLint frame_height = input_frame.rows;

	// 4x4 Projection matrix of current frame
	float projection_matrix[16];

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

	projection_matrix[0] = 2.0f * focal_length_x / frame_width;
	projection_matrix[1] = 0.0f;
	projection_matrix[2] = 0.0f;
	projection_matrix[3] = 0.0f;

	projection_matrix[4] = 0.0f;
	projection_matrix[5] = 2.0f * focal_length_y / frame_height;
	projection_matrix[6] = 0.0f;
	projection_matrix[7] = 0.0f;

	projection_matrix[8] = 1.0f - 2.0f * principle_point_x / frame_width;
	projection_matrix[9] = 2.0f * principle_point_y / frame_height - 1.0f;
	projection_matrix[10] =
		-(clipping_far + clipping_near) / (clipping_far - clipping_near);
	projection_matrix[11] = -1.0f;

	projection_matrix[12] = 0.0f;
	projection_matrix[13] = 0.0f;
	projection_matrix[14] =
		-2.0f * clipping_far * clipping_near / (clipping_far - clipping_near);
	projection_matrix[15] = 0.0f;

	output_projection = glm::make_mat4(projection_matrix);
}

// Turn a cv::Mat into a texture, and return the texture ID as a GLuint
// It's for OpenGL to draw the frame captured by camera
GLuint mat2texture(const cv::Mat& input_image) {
	GLuint texture_id;
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);

	// Give the image to OpenGL
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGB,
		input_image.cols,
		input_image.rows,
		0,
		GL_RGB,
		GL_UNSIGNED_BYTE,
		input_image.data);

	// Set texture interpolation methods for minification and magnification
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

	return texture_id;
}

// Draw the frame (background) captured by my PC's internal camera
void drawBackground(const cv::Mat& input_image, const GLuint& program_id) {
	GLuint background_vertex_array_id;
	glGenVertexArrays(1, &background_vertex_array_id);
	glBindVertexArray(background_vertex_array_id);

	const GLfloat background_vertex_buffer_data[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f,

		1.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f,
		1.0f, -1.0f, 0.0f
	};
	
	const GLfloat background_uv_buffer_data[] = {
		0.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,

		1.0f, 1.0f,
		0.0f, 1.0f,
		1.0f, 0.0f
	};

	GLuint background_vertex_buffer;
	glGenBuffers(1, &background_vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, background_vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(background_vertex_buffer_data),
		background_vertex_buffer_data, GL_STATIC_DRAW);

	GLuint background_uv_buffer;
	glGenBuffers(1, &background_uv_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, background_uv_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(background_uv_buffer_data),
		background_uv_buffer_data, GL_STATIC_DRAW);

	// Use the shaders
	glUseProgram(program_id);

	GLuint texture = mat2texture(input_image);
	GLuint texture_id = glGetUniformLocation(program_id,
		"background_texture_sampler");

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(texture_id, 0);

	// 1st attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, background_vertex_buffer);
	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
		);

	// 2nd attribute buffer : UVs
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, background_uv_buffer);
	glVertexAttribPointer(
		1,
		2,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
		);

	// Draw the triangle !
	glDrawArrays(GL_TRIANGLES, 0, 2 * 3);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	glDeleteTextures(1, &texture);

	/** If they are deleted here, graphics cannot be drawn on background
	glDeleteProgram(program_id);
	glDeleteBuffers(1, &background_uv_buffer);
	glDeleteBuffers(1, &background_vertex_buffer);
	glDeleteVertexArrays(1, &background_vertex_array_id);
	*/
}

// Draw the bunny in ply file with random color
void drawColorBunny(
	const std::vector<glm::vec3>& vertices,
	const glm::mat4& model_matrix,
	const glm::mat4& view_matrix,
	const glm::mat4& projection_matrix,
	const GLuint& program_id) {
	GLuint vertex_array_id;
	glGenVertexArrays(1, &vertex_array_id);
	glBindVertexArray(vertex_array_id);

	GLuint matrix_id = glGetUniformLocation(program_id, "mvp");

	// Give the bunny random colors
	std::vector<glm::vec3> colors;
	for (size_t i = 0; i < vertices.size() * 3; i++) {
		// blending with crimson (a kind of red) and blue
		if (i % 2 == 0) {
			colors.push_back(glm::vec3(0.8f, 0.3f, 0.3f));
		} else {
			colors.push_back(glm::vec3(0.3f, 0.3f, 0.8f));
		}
	}

	GLuint vertex_buffer;
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3),
		&vertices[0], GL_STATIC_DRAW);

	GLuint color_buffer;
	glGenBuffers(1, &color_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
	glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3),
		&colors[0], GL_STATIC_DRAW);

	glUseProgram(program_id);
	glm::mat4 mvp = projection_matrix * view_matrix * model_matrix;
	glUniformMatrix4fv(matrix_id, 1, GL_FALSE, &mvp[0][0]);

	// 1st attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0);

	// 2nd attribute buffer : colors
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
	glVertexAttribPointer(
		1,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0);

	// Draw the triangle !
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	/** If they are deleted here, strange effect will happen
	glDeleteBuffers(1, &color_buffer);
	glDeleteBuffers(1, &vertex_buffer);
	glDeleteProgram(program_id);
	glDeleteVertexArrays(1, &vertex_array_id);
	*/
}

// Draw the bunny in obj file with specular shading
void drawShadingBunny(
	const std::vector<glm::vec3>& vertices,
	const std::vector<glm::vec3>& normals,
	const glm::mat4& model_matrix,
	const glm::mat4& view_matrix,
	const glm::mat4& projection_matrix,
	const GLuint& program_id) {
	GLuint vertex_array_id;
	glGenVertexArrays(1, &vertex_array_id);
	glBindVertexArray(vertex_array_id);

	GLuint matrix_id = glGetUniformLocation(program_id, "MVP");
	GLuint view_matrix_id = glGetUniformLocation(program_id, "V");
	GLuint model_matrix_id = glGetUniformLocation(program_id, "M");

	GLuint vertex_buffer;
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3),
		&vertices[0], GL_STATIC_DRAW);

	GLuint normal_buffer;
	glGenBuffers(1, &normal_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, normal_buffer);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3),
		&normals[0], GL_STATIC_DRAW);

	glUseProgram(program_id);
	GLuint light_id =
		glGetUniformLocation(program_id, "LightPosition_worldspace");

	glUseProgram(program_id);
	glm::mat4 mvp = projection_matrix * view_matrix * model_matrix;
	glUniformMatrix4fv(matrix_id, 1, GL_FALSE, &mvp[0][0]);
	glUniformMatrix4fv(model_matrix_id, 1, GL_FALSE, &model_matrix[0][0]);
	glUniformMatrix4fv(view_matrix_id, 1, GL_FALSE, &view_matrix[0][0]);

	glm::vec3 lightPos = glm::vec3(4, 4, 4);
	glUniform3f(light_id, lightPos.x, lightPos.y, lightPos.z);

	// 1st attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0);

	// 2nd attribute buffer : colors
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, normal_buffer);
	glVertexAttribPointer(
		1,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0);

	// Draw the triangle !
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	/** If they are deleted here, strange effect will happen
	glDeleteBuffers(1, &normal_buffer);
	glDeleteBuffers(1, &vertex_buffer);
	glDeleteProgram(program_id);
	glDeleteVertexArrays(1, &vertex_array_id);
	*/
}
