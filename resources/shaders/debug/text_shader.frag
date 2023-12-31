#version 430 core

// Interpolated values from the vertex shaders
in vec2 uv;

// Ouput data
out vec4 color;

// Values that stay constant for the whole mesh.
uniform sampler2D textureSampler;

void main(){

	color = texture(textureSampler, uv);
}