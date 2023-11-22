#version 430

in float blend;

out vec4 frag;

uniform vec3 empty_color;
uniform vec3 full_color;

void main() {
	if (blend == 0.0) {
		discard;
	}
	frag = vec4((full_color * blend) + (empty_color * (1.0f - blend)), blend);
}