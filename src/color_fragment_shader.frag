#version 330 core

// Interpolated values from the vertex shaders
in vec3 fragment_color;

// Ouput data
out vec3 color;

void main() {
	// Output color = color specified in the vertex shader, 
	// interpolated between all 3 surrounding vertices
	color = fragment_color;

}