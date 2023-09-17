#version 430 core
out vec4 FragColor;

in vec3 tex_coord;

uniform samplerCube skybox;

void main()
{    
    FragColor = texture(skybox, tex_coord);
}