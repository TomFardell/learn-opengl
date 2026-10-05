#version 450 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texture_coords;
layout (location = 2) in vec4 color;

uniform float rotation;

out VertexData {
	vec2 texture_coords;
	vec4 color;
} out_data;

void main() {
	gl_Position= vec4(
		cos(rotation) * position.x - sin(rotation) * position.y,
		sin(rotation) * position.x + cos(rotation) * position.y,
		position.z, 
		1.0f
	);

	out_data.texture_coords = texture_coords;
	out_data.color = color;
}
