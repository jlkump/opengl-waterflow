#version 430

in vec3 color;
in float blend;

out vec3 frag;

void main() {
	frag = color * blend;
}