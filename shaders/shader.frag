#version 450 core

in VertexData {
	vec2 texture_coords;
	vec4 color;
} in_data;

uniform sampler2D texture_tiles;
uniform sampler2D texture_container;
uniform sampler2D texture_cat;

uniform float cat_mix;

out vec4 color;

void main() {
	color = in_data.color * mix(
		mix (
			texture(texture_tiles, in_data.texture_coords),
			texture(texture_container, in_data.texture_coords),
			0.5f
		),
		texture(texture_cat, in_data.texture_coords),
		cat_mix
	);
}
