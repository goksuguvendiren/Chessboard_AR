#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 transform;
uniform mat4 view_proj;

out vec4 normal;
out vec3 world_pos;

void main()
{
//    normal = aNormal;
    normal = vec4(normalize(vec3(transpose(inverse(transform)) * vec4(aNormal, 0.0f))), 0);
//    normal = normalize(mat3(transpose(inverse(transform))) * aNormal);
    world_pos = vec3(transform * vec4(aPos, 1.0f));
    gl_Position = view_proj * transform * vec4(aPos, 1.0f);
//    gl_Position = vec4(aPos, 1.0f);
}
