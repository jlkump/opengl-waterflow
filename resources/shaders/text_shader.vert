#version 430

layout(location = 0) in vec2 ls_pos; // Text position in homogenous coordinates
layout(location = 1) in vec2 ls_tex; // Text uvs

out vec2 uv;

void main()
{
	uv = ls_tex;
	gl_Position = vec4(ls_pos, 0.0, 1.0);
}