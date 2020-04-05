#pragma once

#ifndef GRAPHICS_UTILITY
#define GRAPHICS_UTILITY

#include<vector>
#include<string>

#include <glm/glm.hpp>

// Load ply file
// If succeed, give all the vertices and face indices, and return true
// If fail, return false
bool loadPly(
	const std::string& input_filename,
	std::vector<glm::vec3>& output_vertices);

#endif // !GRAPHICS_UTILITY
