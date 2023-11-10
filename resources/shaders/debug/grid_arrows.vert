#version 430

layout(location = 0) in vec3 ls_arrow_vert;		// Iterates through the 18 different vertices
layout(location = 1) in vec3 ws_arrow_position; // Same for the 18 iterations
layout(location = 2) in vec3 color;				// Same for the 18 iterations
layout(location = 3) in vec3 grid_index;		// UV index into the grid of velocities, same for the 18 iterations

out vec3 arrow_color;

uniform sampler3D velocities; 
uniform mat4 proj_view;
uniform mat4 view;

void main() {
	vec3 view_dir = view[2].xyz;
	vec3 up = texture(velocities, grid_index).xyz;
	vec3 right = cross(view_dir, up);

	view_dir = vec3(0, 0, 0.5);
	up = vec3(0, 0.5, 0);
	right = vec3(0.5, 0, 0);

//	model[2] = vec4(view_dir, 0.0);
//	model[1] = vec4(up, 0.0);
//	model[0] = vec4(right, 0.0);
//	model[3] = vec4(ws_arrow_position, 1.0);
	mat4 model = mat4( 
			1.0, 0.0, 0.0, 0.0,
			0.0, 1.0, 0.0, 0.0,
			0.0, 0.0, 1.0, 0.0,
			ws_arrow_position.x, ws_arrow_position.y, ws_arrow_position.z, 1.0);

	gl_Position = proj_view * model * vec4(ls_arrow_vert, 1.0);
	// gl_Position = vec4(ls_arrow_vert, 1.0);

	arrow_color = color;
}