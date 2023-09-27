#version 430 core

layout(location = 0) in vec3 ls_particle_quad_pos;
layout(location = 1) in vec3 ws_particle_pos; // Center of the particle

out vec2 uv;
out vec3 vs_pos;

uniform vec3 ws_camera_up;
uniform vec3 ws_camera_right;
uniform float particle_radius;

uniform mat4 proj_view;
uniform mat4 view;

void main() {
	
	vec3 ws_vertex_pos = ws_particle_pos 
			+ ws_camera_right * ls_particle_quad_pos.x * particle_radius 
			+ ws_camera_up * ls_particle_quad_pos.y * particle_radius;

	gl_Position = proj_view * vec4(ws_vertex_pos, 1.0);
	vs_pos = (proj_view * vec4(ws_vertex_pos, 1.0)).xyz;
	uv = (ls_particle_quad_pos.xy + vec2(1.0, 1.0)) / 2;
}