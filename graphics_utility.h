#pragma once

#ifndef GRAPHICS_UTILITY
#define GRAPHICS_UTILITY

#include <vector>
#include <string>

#define GLEW_STATIC
#include <GL/glew.h>
#include <glm/glm.hpp>

// Load ply file
// If succeed, give all the vertices, and return true
// If fail, return false
bool loadPly(
	const std::string& input_filename,
	std::vector<glm::vec3>& output_vertices);

// Load ply file
// If succeed, give all the vertices and normals, and return true
// If fail, return false
bool loadObj(
	const std::string& input_filename,
	std::vector<glm::vec3>& output_vertices,
	std::vector<glm::vec3>& output_normals);

// Load shaders
GLuint loadShaders(
	const char* vertex_file_path,
	const char* fragment_file_path);

#endif // !GRAPHICS_UTILITY
