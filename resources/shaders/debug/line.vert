#version 430

layout(location = 0) in vec3 ls_quad_vert;
layout(location = 1) in vec3 ws_line_pos;
layout(location = 2) in vec3 ws_line_end;
layout(location = 3) in vec3 line_color;

out vec3 color;

uniform float line_thickness;

uniform mat4 view;
uniform mat4 proj_view;

void main()
{
	color = line_color;
	vec3 ws_cam_right = {view[0][0], view[1][0], view[2][0]};
	ws_cam_right *= line_thickness;
	vec3 ws_cam_up = {view[0][1], view[1][1], view[2][1]};
	vec3 line_dir = ws_line_end - ws_line_end;
	vec3 right = cross(ws_cam_up, line_dir);
	vec3 forward = cross(right, line_dir);
//	mat4 model = mat4(
//		right.x, right.y, right.z, 0.0,
//		line_dir.x, line_dir.y, line_dir.z, 0.0,
//		forward.x, forward.y, forward.z, 0.0,
//		ws_line_pos.x, ws_line_pos.y, ws_line_pos.z, 1.0
//	);
	mat4 model = mat4(
		line_thickness, 0.0, 0.0, 0.0,
		0.0, line_thickness, 0.0, 0.0,
		0.0, 0.0, line_thickness, 0.0,
		ws_line_pos.x, ws_line_pos.y, ws_line_pos.z, 1.0
	);
	gl_Position = model * vec4(ls_quad_vert, 1.0);
//	gl_Position = proj_view * model * vec4(ws_pos, 1.0);
}