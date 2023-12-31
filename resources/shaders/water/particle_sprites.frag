#version 430 core

in vec2 uv;
in vec3 vs_pos;

layout(location = 0) out vec3 Depth;

uniform vec3 vs_cam_pos;
uniform float particle_radius;

uniform mat4 proj;

void main() {
	// Code used from the 2010 GDC talk on realtime water rendering
	vec3 N;
	N.xy = (uv.xy * 2.0 - vec2(1.0));
	float r = dot(N.xy, N.xy);
	if (r > 1.0) discard;
	N.z = -sqrt(1.0 - r);
	N = normalize(N);
	
	// Depth calculation
	vec4 pixel_pos = vec4(N * particle_radius * 2, 1.0);
	vec4 hs_pos = proj * pixel_pos;
	Depth = vec3(vs_pos.z - hs_pos.z);
}