#version 430 core

in vec2 uv;

// layout(location=1) out vec3 depth;
out vec3 Depth;

uniform sampler2D depth_sampler;

uniform float filter_radius; // Using a float, but should be a integer as input. Just done to prevent casting

void main() {
	// Code used and modified from the GDC 2010 realtime water rendering slides
	// (https://developer.download.nvidia.com/presentations/2010/gdc/Direct3D_Effects.pdf)
	float d = texture(depth_sampler, uv).r;
	ivec2 texDimen = textureSize(depth_sampler, 0);
	float delta_x = 1.0 / texDimen.x;
	float delta_y = 1.0 / texDimen.y;

	int sample_count = 0;
	for (float x = -filter_radius; x <= filter_radius; x += 1.0) 
	{
		for (float y = -filter_radius; y <= filter_radius; y += 1.0) 
		{
			float d_sample = texture(depth_sampler, uv + vec2(x * delta_x, y * delta_y)).r;
			if (!(d_sample - 1e-5 <= 0)) 
			{
				d += d_sample;
				sample_count += 1;
			} 
		}
	}
	if (sample_count > 0 ) 
	{
		d /= sample_count;
	}
	Depth = vec3(d);
}