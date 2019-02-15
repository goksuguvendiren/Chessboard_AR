#version 330 core

uniform sampler2D tex;
out vec4 FragColor;

void main()
{
    vec2 uv = gl_FragCoord.xy / vec2(1280, 720);
    FragColor = vec4(texture(tex, uv));
}
