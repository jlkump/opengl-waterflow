#version 430

in vec2 uv;
in vec3 particle_color;

out vec3 color;

void main() {
	// Code used from the 2010 GDC talk on realtime water rendering
	vec3 N;
	N.xy = (uv.xy * 2.0 - vec2(1.0));
	float r = dot(N.xy, N.xy);
	if (r > 1.0) discard;

	color = particle_color;
}