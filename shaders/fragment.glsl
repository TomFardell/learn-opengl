#version 450 core

in VertexData {
	vec4 color;
} in_data;

uniform float brightness;

out vec4 color;

void main() {
	color = vec4(brightness * in_data.color.rgb, in_data.color.a);
}
