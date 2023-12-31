#version 430 core

layout(location = 0) in vec3 ls_particle_quad_pos;
layout(location = 1) in vec2 pos_index; // Center of the particle, as a uv index into the particle positions texture

out vec2 uv;
out vec3 vs_pos;

// Since the compute shader updates a texture, don't process the texture on the CPU,
// just pass it directly to the GPU and provide indices
uniform sampler2D ws_particle_positions; 

uniform vec3 ws_camera_up;
uniform vec3 ws_camera_right;
uniform float particle_radius;

uniform mat4 proj_view;

void main() {
	
	vec3 ws_vertex_pos = texture(ws_particle_positions, pos_index).xyz
			+ ws_camera_right * ls_particle_quad_pos.x * particle_radius 
			+ ws_camera_up * ls_particle_quad_pos.y * particle_radius;

	gl_Position = proj_view * vec4(ws_vertex_pos, 1.0);
	vs_pos = (proj_view * vec4(ws_vertex_pos, 1.0)).xyz;
	uv = (ls_particle_quad_pos.xy + vec2(1.0, 1.0)) / 2;
}