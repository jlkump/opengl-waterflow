#version 430 core

in vec2 uv;

out vec3 FragColor;

uniform sampler2D depth_tex;
// uniform sampler2D bg_tex;
uniform samplerCube skybox;
uniform vec3 ws_cam_pos;
uniform vec3 ws_light_dir;
uniform vec3 diffuse_color;

uniform mat4 inv_proj;
uniform mat4 inv_view;

vec3 GetViewPos(vec2 tex_coord) {
	vec4 hs_pos = vec4 (
		2.0 * tex_coord.x - 1.0,
		2.0 * tex_coord.y - 1.0,
		2.0 * texture(depth_tex, tex_coord).r - 1.0,
		1.0);
	vec4 vs_pos = (inv_proj * hs_pos);
	return vs_pos.xyz / vs_pos.w;
}

vec3 GetWorldPos(vec2 tex_coord) {
	return (inv_view * vec4(GetViewPos(tex_coord), 1.0)).rgb;
}

vec3 GetNormal() {
	float depth = texture(depth_tex, uv).r;
	
	ivec2 texDimen = textureSize(depth_tex, 0);
	
	vec3 vs_pos = GetViewPos(uv);
	
	vec3 ddx = GetViewPos(uv + vec2(1, 0) / texDimen) - vs_pos;
	vec3 ddx2 = vs_pos - GetViewPos(uv + vec2(-1, 0) / texDimen);
	if (abs(ddx.z) > abs(ddx2.z)) {
		ddx = ddx2;
	}
	
	vec3 ddy = GetViewPos(uv + vec2(0, 1) / texDimen) - vs_pos;
	vec3 ddy2 = vs_pos - GetViewPos(uv + vec2(0, -1) / texDimen);
	if (abs(ddy.z) > abs(ddy2.z)) {
		ddy = ddy2;
	}
	vec3 norm = normalize(cross(ddx, ddy));
	if (norm.z + 1e-7 >= 1.0) discard; // Bad culling, should use stencil mask instead, but lazy atm :P
	return norm;
}

void main() {

	vec3 N = GetNormal(); // TODO: shading.

	// Use blin-phong illumination
	// Introduce reflections
    vec3 I = normalize(ws_cam_pos - GetWorldPos(uv));
	vec3 R = reflect(I, N);
	float diffuse_amount = dot(N, ws_light_dir) * 0.5 + 0.5;

	float base_reflectance = 0.6; // 2 * dot(N, ws_light_dir) * ws_light_dir - ;
	float reflection_amount = base_reflectance + (1.0 - base_reflectance) * pow(1.0 - dot(N, I), 5);
	vec3 reflection_color = texture(skybox, R).rgb;

	FragColor = diffuse_color * diffuse_amount * .1 + reflection_amount * reflection_color;
}