#version 430 core

in vec2 uv;
in vec3 vs_pos;

out vec3 FragColor;

uniform vec3 vs_cam_pos;
uniform vec3 vs_light_dir;
uniform float particle_radius;

uniform mat4 proj;

void main() {
	// Code used from the 2010 GDC talk on realtime water rendering
	vec3 N;
	N.xy = (uv.xy * 2.0 - vec2(1.0));
	float r = dot(N.xy, N.xy);
	if (r > 1.0) discard;
	N.z = -sqrt(1.0 - r);

	// Depth calculation
	vec4 pixel_pos = vec4(vs_pos + N * particle_radius, 1.0);
	vec4 hs_pos = proj * pixel_pos;
	gl_FragDepth = hs_pos.z / hs_pos.w;
	FragColor = vec3(hs_pos.z / hs_pos.w, hs_pos.z / hs_pos.w, hs_pos.z / hs_pos.w);

	// Fragment color
	float diffuse = max(0.0, dot(N, vs_light_dir));
	// FragColor = diffuse * vec3(0.0, 0.7, 0.4);

	// gl_FragColor = vec4(diffuse * vec3(0.0, 0.7, 0.4), 1.0);
}