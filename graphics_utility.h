#pragma once

#ifndef GRAPHICS_UTILITY
#define GRAPHICS_UTILITY

#include<vector>
#include<string>

// Load ply file
// If succeed, give all the vertices and face indices, and return true
// If fail, return false
bool loadPly(
	const std::string& input_filenamee,
	std::vector<float>& output_vertices,
	std::vector<int>& output_face_indices);

#endif // !GRAPHICS_UTILITY
