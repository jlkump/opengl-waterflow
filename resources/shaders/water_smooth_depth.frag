#version 430 core

in vec2 uv;

// layout(location=1) out vec3 depth;
out vec3 Depth;

uniform sampler2D depth_sampler;

// Reference video https://www.youtube.com/watch?v=7FP7ndMEfsc&t=784s
// Reference code https://github.com/tranvansang/bilateral-filter/blob/master/fshader.frag
uniform float blur_depth_falloff; // Sigma l
uniform float blur_scale; // Sigma s
uniform float filter_radius; // Using a float, but should be a integer as input. Just done to prevent casting


void main() {
	// Code used and modified from the GDC 2010 realtime water rendering slides
	// (https://developer.download.nvidia.com/presentations/2010/gdc/Direct3D_Effects.pdf)
	float d = texture(depth_sampler, uv).x; // Arbitrarly pick a coord
	float d_lum = d;
	ivec2 texDimen = textureSize(depth_sampler, 0);

	vec4 d_sum = vec4(0);
	float d_wsum = 0;

	float sig_s = max(blur_scale, 1e-5);
	float sig_l = max(blur_depth_falloff, 1e-5);

	float frac_s = -1.0 / (2.0 * sig_s * sig_s);
	float frac_l = -1.0 / (2.0 * sig_l * sig_l);

	// For tryin gaussian, just for testing really
	// vec4 g_sum = vec4(0);
	// int iteration = 0;

	for (float x = -filter_radius; x <= filter_radius; x += 1.0) {
		for (float y = -filter_radius; y <= filter_radius; y += 1.0) {
			vec2 offset = vec2(x, y);
			vec4 d_sample = texture(depth_sampler, uv + offset / texDimen);

			float diff_dist = length(offset); // Difference in distance from current point
			float diff_lum = length(d_sample.xyz) - d_lum; // Difference in luminosity

			float w_s = exp(frac_s * float(diff_dist * diff_dist));
			float w_l = exp(frac_l * float(diff_lum * diff_lum));
			float w = w_s * w_l;
			d_wsum += w;
			d_sum += d_sample * w;

			// iteration += 1;
			// g_sum += d_sample;
		}
	}

	if (d_wsum > 0.0) {
		d_sum /= d_wsum;
	}
	Depth = d_sum.rgb;

	// Depth = (g_sum / iteration).rgb;
}