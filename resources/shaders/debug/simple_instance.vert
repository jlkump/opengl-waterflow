#version 430

layout(location = 0) in vec3 ls_vert;
layout(location = 1) in vec3 in_color;
layout(location = 2) in mat4 model; // Put this one last, since it reserves 4 locations, one for each column of the matrix

out vec3 color;
out float blend;

uniform mat4 proj_view;

void main()
{
	color = in_color;
	blend = (ls_vert.y / 2.0) + 0.5;
	gl_Position = proj_view * model * vec4(ls_vert, 1.0);
}