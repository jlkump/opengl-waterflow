#version 430

layout(location = 0) in vec3 ws_pos; // Text position in homogenous coordinates
layout(location = 1) in vec3 in_color; // Text uvs

out vec3 color;

uniform mat4 proj_view;

void main()
{
	color = in_color;
	mat4 model = mat4(
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 0.999, 0.0,
		0.0, 0.0, 0.0, 1.0
	);
	gl_Position = proj_view * model * vec4(ws_pos, 1.0);
}