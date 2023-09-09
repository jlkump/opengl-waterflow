#version 430 core

layout (location = 0) in vec3 vert;
layout (location = 1) in vec2 uv;

out vec2 UV;

void main () 
{
	gl_Position = vec4(vert, 1);
	UV = uv;
}