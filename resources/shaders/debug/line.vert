#version 430

layout(location = 0) in vec3 ws_pos; // Text position in homogenous coordinates
layout(location = 1) in vec3 in_color; // Text uvs

out vec3 color;

uniform mat4 proj_view;

void main()
{
	color = in_color;
	gl_Position = proj_view * vec4(ws_pos, 1.0);
}