#version 430 core

in vec2 uv;
in vec3 center;

out vec3 FragColor;

uniform float particle_radius;
uniform vec3 ws_light_dir;

void main() {
	// vec3 N;
	// N.xy = (uv.xy * 2.0 - vec2(1.0));
	// float r = dot(N.xy, N.xy);
	// if (r > particle_radius) discard;
	// N.z = -sqrt(1.0 - r);
	// float diffuse = max(0.0, dot(N, ws_light_dir));
	// FragColor = diffuse * vec3(0.0, 0.1, 0.4);

	FragColor = vec3(0.8, 0.1, 0.4);

}