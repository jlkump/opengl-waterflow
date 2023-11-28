#version 430 core

layout(location = 0) in vec3 ls_particle_quad_pos;
layout(location = 1) in vec3 particle_pos;
layout(location = 2) in vec3 particle_in_color;

out vec2 uv;
out vec3 particle_color;

uniform vec3 ws_camera_up;
uniform vec3 ws_camera_right;
uniform float particle_radius;

uniform mat4 proj_view;

void main() {
	vec3 ws_vertex_pos = particle_pos
			+ ws_camera_right * ls_particle_quad_pos.x * particle_radius 
			+ ws_camera_up * ls_particle_quad_pos.y * particle_radius;

	gl_Position = proj_view * vec4(ws_vertex_pos, 1.0);
	uv = (ls_particle_quad_pos.xy + vec2(1.0, 1.0)) / 2;
	particle_color = particle_in_color;
}