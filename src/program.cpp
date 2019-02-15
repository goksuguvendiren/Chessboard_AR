//
// Created by Göksu Güvendiren on 2019-01-19.
//

#include <program.hpp>
#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

grt::program::program()
{
    shader_program = glCreateProgram();
}

grt::program::~program()
{
}

void grt::program::Attach(const grt::vertex_shader& shader)
{
    glAttachShader(shader_program, shader.GetShader());
}

void grt::program::Attach(const grt::fragment_shader &shader)
{
    glAttachShader(shader_program, shader.GetShader());
}

void grt::program::Link()
{
    glLinkProgram(shader_program);
}

void grt::program::SetVariable(const std::string& variable_name, const glm::mat4& value)
{
    glUseProgram(shader_program);

    auto variable_location = glGetUniformLocation(shader_program, variable_name.c_str());
    glUniformMatrix4fv(variable_location, 1, GL_FALSE, glm::value_ptr(value));
}

void grt::program::SetVariable(const std::string& variable_name, const glm::vec3& value)
{
    glUseProgram(shader_program);

    auto variable_location = glGetUniformLocation(shader_program, variable_name.c_str());
    glUniform3fv(variable_location, 1, glm::value_ptr(value));
}

void grt::program::SetVariable(const std::string& variable_name, int value)
{
    glUseProgram(shader_program);

    auto variable_location = glGetUniformLocation(shader_program, variable_name.c_str());
    glUniform1i(variable_location, value);
}