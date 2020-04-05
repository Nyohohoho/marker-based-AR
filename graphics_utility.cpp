#include "graphics_utility.h"

#include<vector>
#include<string>
#include<fstream>

// Load ply file
// If succeed, give all the vertices and face indices, and return true
// If fail, return false
bool loadPly(
	const std::string& input_filename,
	std::vector<glm::vec3>& output_vertices) {
	// Use a stream to read ply file
	std::ifstream read_file(input_filename, std::ios::in);

	// If faill to open the file, return false
	if (!read_file.is_open()) {
		return false;
	}

	// If the output vector is not empty, clear it
	if (!output_vertices.empty()) {
		output_vertices.clear();
	}

	// Used to record the header in ply file
	std::string header;
	// Used to recorder the number of vertex and the number of face
	size_t num_of_vertex = 0;
	size_t num_of_face = 0;
	while (!read_file.eof()) {
		// Read the first header of a line
		read_file >> header;

		// The following headers do not provide important information
		// so skip them and go to next line
		if (header == "ply" || header == "format" ||
			header == "comment" || header == "property") {
			std::string line;
			std::getline(read_file, line);
			continue;
		}

		// If the header indicates "element"
		if (header == "element") {
			// Read the next word (vertex or face)
			std::string keyword;
			read_file >> keyword;
			if (keyword == "vertex") {
				read_file >> num_of_vertex;

				// Skip the rest and go to next line
				std::string line;
				std::getline(read_file, line);
				continue;
			}
			if (keyword == "face") {
				read_file >> num_of_face;

				// Skip the rest and go to next line
				std::string line;
				std::getline(read_file, line);
				continue;
			}
		}

		// If the header comes to the end, start reading vertices and faces
		if (header == "end_header") {
			// Skip the rest and go to next line
			std::string line;
			std::getline(read_file, line);

			// Store the coordinates of vertex
			std::vector<glm::vec3> vertex_coordinates;
			// The index indicates the vertex that a face has
			std::vector<int> face_vertex_index;

			for (size_t i = 0; i < num_of_vertex; i++) {
				float vertex_x, vertex_y, vertex_z;
				// x-coordinate
				read_file >> vertex_x;
				// y-coordinate
				read_file >> vertex_y;
				// z-coordinate
				read_file >> vertex_z;

				vertex_coordinates.push_back(glm::vec3(
					vertex_x, vertex_y, vertex_z));

				// confidence and intensity are not needed
				// so skip them and go to next line
				std::getline(read_file, line);
			}

			size_t face_info;
			for (size_t i = 0; i < num_of_face; i++) {
				// the number of vertex in one face
				read_file >> face_info;
				if (face_info == 3) {
					// vertex index 1
					read_file >> face_info;
					face_vertex_index.push_back(face_info);
					// vertex index 2
					read_file >> face_info;
					face_vertex_index.push_back(face_info);
					// vertex index 3
					read_file >> face_info;
					face_vertex_index.push_back(face_info);
				}
			}

			for (size_t i = 0; i < face_vertex_index.size(); i++) {
				size_t current_index = face_vertex_index[i];
				output_vertices.push_back(vertex_coordinates[current_index]);
			}

			// finish reading and succeed
			return true;
		}
	}

	return false;
}

