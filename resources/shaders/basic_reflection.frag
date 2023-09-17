#version 430

in vec3 ws_pos;
in vec3 ws_norm;
in vec2 uv;

out vec4 FragColor;
layout(binding = 0) uniform samplerCube skybox;

uniform vec3 ws_cam_pos;

void main()
{             
    vec3 I = normalize(ws_pos - ws_cam_pos);
    vec3 R = reflect(I, normalize(ws_norm));
    FragColor = vec4(texture(skybox, R).rgb, 1.0);
}