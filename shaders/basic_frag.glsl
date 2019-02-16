#version 330 core

uniform vec3 obj_color;
uniform vec3 light_intensity;
uniform vec3 light_position;
uniform vec3 camera_position;

in vec4 normal;
in vec3 world_pos;
out vec4 FragColor;

void main()
{
    //ambient
    vec3 ambient = vec3(0.25, 0.25, 0.25);

    //diffuse
    vec3 w_i = normalize(light_position - world_pos);
    vec3 diffuse = max(0.0f, dot(w_i, vec3(normal))) * light_intensity;

    //specular
    vec3 view_direction = normalize(camera_position - world_pos);
    vec3 h   = normalize(view_direction + w_i);
    vec3 specular = pow(max(0.0f, dot(vec3(normal), h)), 10) * light_intensity;

    float distance = length(light_position - world_pos);
    diffuse  /= distance * distance;
    specular /= distance * distance;

    vec3 result = (ambient + diffuse + specular) * obj_color;

    FragColor = vec4(result, 1.f);
}
