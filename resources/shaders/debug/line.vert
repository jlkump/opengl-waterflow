#version 430

layout(location = 0) in vec3 ls_quad_vert;
layout(location = 1) in vec3 line_color;
layout(location = 2) in mat4 line_model; // Put this one last, since it reserves 4 locations, one for each column of the matrix

out vec3 color;
out float blend;

uniform mat4 proj_view;

void main()
{
	color = line_color;
	blend = (ls_quad_vert.y / 2.0) + 0.5;
	gl_Position = proj_view * line_model * vec4(ls_quad_vert, 1.0);
}