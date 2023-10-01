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

vec3 CalculateNorm(vec2 tex_coord, vec2 texDimen) {
	int dd_diff = 2;
	vec3 vs_pos = GetViewPos(tex_coord);
	vec3 ddx = GetViewPos(tex_coord + vec2(dd_diff, 0) / texDimen) - vs_pos;
	vec3 ddx2 = vs_pos - GetViewPos(tex_coord + vec2(-dd_diff, 0) / texDimen);
	if (abs(ddx.z) > abs(ddx2.z)) {
		ddx = ddx2;
	}
	
	vec3 ddy = GetViewPos(uv + vec2(0, dd_diff) / texDimen) - vs_pos;
	vec3 ddy2 = vs_pos - GetViewPos(uv + vec2(0, -dd_diff) / texDimen);
	if (abs(ddy.z) > abs(ddy2.z)) {
		ddy = ddy2;
	}

	return normalize(cross(ddx, ddy));
}

vec3 GetNormal() {
	float depth = texture(depth_tex, uv).r;
	if (depth - 1e-5 <= 0) discard;
	ivec2 texDimen = textureSize(depth_tex, 0);
	vec3 norm = CalculateNorm(uv, texDimen);
			
	// if (norm.z + 1e-5 >= 1) discard;
	norm =	(norm 
			+ CalculateNorm(uv + vec2(1, 0) / texDimen, texDimen) 
			+ CalculateNorm(uv + vec2(0, 1) / texDimen, texDimen) 
			+ CalculateNorm(uv + vec2(-1, 0) / texDimen, texDimen) 
			+ CalculateNorm(uv + vec2(0, -1) / texDimen, texDimen))
			/ 5;
	// norm.y = -norm.y;
	// norm.x = -norm.x;
	return norm;
}

void main() {

	vec3 N = -GetNormal();
	
	// Simple diffuse and reflection illumination/shading
    vec3 I = normalize(ws_cam_pos - GetWorldPos(uv));
	vec3 R = reflect(I, N);
	R.y = -R.y;
	vec3 reflection_color = texture(skybox, R).rgb;
	float default_reflection = 0.3;
	float reflection_amount = default_reflection + ((1.0 - default_reflection) * (1 - dot(I, N)));
	float diffuse_amount = (dot(N, ws_light_dir) * 0.5 + 0.5) * 0.1;

	vec3 refraction_color = texture(skybox, refract(I, -N, 1.33)).rgb;

	FragColor = reflection_color * reflection_amount 
			+ diffuse_color * diffuse_amount
			+ refraction_color * (1 - reflection_amount);
}