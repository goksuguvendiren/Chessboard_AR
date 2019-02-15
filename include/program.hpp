//
// Created by Göksu Güvendiren on 2019-01-19.
//

#pragma once

#include <string>
#include "vertex_shader.hpp"
#include "fragment_shader.hpp"

namespace grt
{
    class program
    {
        unsigned int shader_program;

    public:
        program();
        ~program();

        void Attach(const grt::vertex_shader& shader);
        void Attach(const grt::fragment_shader& shader);

        void Link();

        void SetVariable(const std::string& variable_name, const glm::mat4& value);
        void SetVariable(const std::string& variable_name, const glm::vec3& value);
        void SetVariable(const std::string& variable_name, int value);

        unsigned int GetProgram() const { return shader_program; }
    };
}