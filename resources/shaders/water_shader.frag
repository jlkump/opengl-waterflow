#version 430 core

in vec2 uv;

out vec3 FragColor;

uniform sampler2D depth_tex;

uniform vec4 viewport; // viewport.x is max width, y is max height
uniform mat4 inv_proj;

vec3 GetEyePos(vec2 tex_coord) {
	vec4 hs_pos = vec4 (
		2.0 * (tex_coord.x - viewport.x) / viewport.x - 1.0,
		2.0 * (tex_coord.y - viewport.y) / viewport.y - 1.0,
		2.0 * texture(depth_tex, tex_coord).r - 1.0,
		1.0);
	vec4 vs_pos = (inv_proj * hs_pos);
	return vs_pos.xyz / vs_pos.w;
}

void main() {
	float depth = texture(depth_tex, uv).r;
	// Stencil shader should discard drawing outside the water

	vec3 vs_pos = GetEyePos(uv);

	vec3 ddx = GetEyePos(uv + vec2(1, 0)) - vs_pos;
	vec3 ddx2 = vs_pos - GetEyePos(uv + vec2(-1, 0));
	if (abs(ddx.z) > abs(ddx2.z)) {
		ddx = ddx2;
	}

	vec3 ddy = GetEyePos(uv + vec2(0, 1)) - vs_pos;
	vec3 ddy2 = vs_pos - GetEyePos(uv + vec2(0, -1));
	if (abs(ddy.z) > abs(ddy2.z)) {
		ddy = ddy2;
	}
	vec3 norm = normalize(cross(ddx, ddy));
	FragColor = norm; // TODO: shading.
}