
#version 430

in vec3 ws_pos;
in vec3 ws_norm;
in vec2 uv;

layout(location = 0) out vec4 fragColor;

layout(binding = 0) uniform sampler2D tex_sampler;
uniform vec3 ws_cam_pos;

void main()
{   
	vec3 n = normalize(ws_norm);
	vec3 frag_pos = normalize(ws_cam_pos - ws_pos);
	
	vec3 finalColor = texture(tex_sampler, uv).xyz;
	finalColor *= max(0.0f, dot(n, frag_pos));
	
	fragColor = vec4(finalColor, 1.0f);
}