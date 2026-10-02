#version 450 core

in VertexData {
	vec2 texture_coords;
	vec4 color;
} in_data;

uniform sampler2D texture_data;

out vec4 color;

void main() {
	color = texture(texture_data, in_data.texture_coords) * in_data.color;
}
