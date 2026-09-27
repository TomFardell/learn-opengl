#version 450 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec4 color_vert;

out vec4 color_frag;

void main() {
	gl_Position = vec4(position, 1.0f);
	color_frag = color_vert;
}
