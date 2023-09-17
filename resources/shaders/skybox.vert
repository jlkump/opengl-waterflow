#version 430 core
layout (location = 0) in vec3 aPos;

out vec3 tex_coord;

uniform mat4 proj_view;

void main()
{
    tex_coord = aPos;
    vec4 pos = proj_view * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}  