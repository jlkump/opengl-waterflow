#version 430 core

layout (location = 0) in vec3 pos;

out vec2 uv;

void main() {
	uv = (pos.xy + vec2(1.0)) / 2;
	gl_Position = vec4(pos, 1.0);
}