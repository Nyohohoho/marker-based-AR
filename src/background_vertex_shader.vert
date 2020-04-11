#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertex_position_modelspace;
layout(location = 1) in vec2 vertex_UV;

// Output data ; will be interpolated for each fragment.
out vec2 UV;

void main() {
	// Output position of the backgroud's vertex, in clip space : position
	gl_Position =  vec4(vertex_position_modelspace, 1);
	
	// UV of the vertex. No special space for this one.
	UV = vertex_UV;
}

