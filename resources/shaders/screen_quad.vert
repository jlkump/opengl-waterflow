#version 430 core

layout (location = 0) vec3 pos;

out vec2 uv;

void main() {
	uv = pos.xy;
	gl_Position = vec4(pos, 1.0);
}