#version 330 core

uniform vec3 obj_color;
uniform vec3 light_color;
uniform vec3 light_position;
uniform vec3 camera_position;

uniform sampler2D tex;

in vec4 normal;
in vec3 world_pos;
out vec4 FragColor;

void main()
{
    vec2 uv = gl_FragCoord.xy / vec2(960, 540);
    FragColor = vec4(texture(tex, uv));
}
