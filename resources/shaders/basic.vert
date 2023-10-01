#version 430

layout(location = 0) in vec3 ls_pos; // Local space of the model vertices
layout(location = 1) in vec3 ls_norm; // Local space of the model normals
layout(location = 2) in vec3 ls_tex; // Local space of the model uvs (texture coordinates)

out vec3 ws_pos;
out vec3 ws_norm;
out vec2 uv;
	
uniform mat4 model;
uniform mat4 proj_view;
uniform mat3 norm_matrix;
	
void main()
{
	ws_pos = vec3(model * vec4(ls_pos, 1.0f));
    ws_norm = norm_matrix * ls_norm;
    uv = ls_tex.xy;
	gl_Position = proj_view * model * vec4(ls_pos, 1.0f);
}
