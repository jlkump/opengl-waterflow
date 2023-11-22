#version 430

layout(location = 0) in vec3 ls_vert;
layout(location = 1) in float cell_float;
layout(location = 2) in mat4 cell_model; // Put this one last, since it reserves 4 locations, one for each column of the matrix

out float blend;

uniform mat4 proj_view;


void main()
{
	blend = cell_float;	
	gl_Position = proj_view * cell_model * vec4(ls_vert, 1.0);
}