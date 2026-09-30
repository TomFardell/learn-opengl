#version 450 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec4 color;

uniform float rotation;

out VertexData {
	vec4 color;
} out_data;

void main() {
	gl_Positionj= vec4(
		cos(rotation) * position.x - sin(rotation) * position.y,
		sin(rotation) * position.x + cos(rotation) * position.y, 
		position.z, 
		1.0f
	);
	out_data.color = color;
}
