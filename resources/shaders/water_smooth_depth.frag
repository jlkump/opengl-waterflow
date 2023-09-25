#version 430 core

in vec2 uv;

// layout(location=1) out vec3 depth;
out vec3 Depth;

uniform sampler2D depth_sampler;

uniform float blur_depth_falloff;
uniform float filter_radius;
uniform float blur_scale;

void main() {
	// Code used from the GDC 2010 realtime water rendering slides
	// (https://developer.download.nvidia.com/presentations/2010/gdc/Direct3D_Effects.pdf)
	float d = texture(depth_sampler, uv).x; // Arbitrarly pick a coord

	float d_sum = 0;
	float d_wsum = 0;
	

	vec2 blurDir = vec2(0.0, 1.0);
	// TODO fix smoothing
	for (float x = -filter_radius; x <= filter_radius; x += 1.0) {

		float d_sample = texture(depth_sampler, uv + x * blurDir).x;

		float d_r = x * blur_scale;
		float d_w = exp(-d_r * d_r);

		float d_r2 = (d_sample - d) * blur_depth_falloff;
		float d_g = exp(-d_r2 * d_r2);

		d_sum += d_sample * d_w * d_g;
		d_wsum += d_w * d_g;
	}

	if (d_wsum > 0.0) {
		d_sum /= d_wsum;
	}

	Depth = vec3(d);
}