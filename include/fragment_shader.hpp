//
// Created by Göksu Güvendiren on 2019-01-19.
//

#pragma once

#include <string>

namespace grt
{
    class fragment_shader
    {
        int shader;
        void check_status(unsigned int flag)
        {
            int  success;
            char infoLog[512];
            glGetShaderiv(shader, flag, &success);

            if(!success)
            {
                glGetShaderInfoLog(shader, 512, nullptr, infoLog);
                std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << "\n";
            }
        }

    public:
        fragment_shader(const std::string& source)
        {
            shader = glCreateShader(GL_FRAGMENT_SHADER);
            const auto addr = source.c_str();

            glShaderSource(shader, 1, &addr, nullptr);
            glCompileShader(shader);

            check_status(GL_COMPILE_STATUS);
        }

        ~fragment_shader()
        {
            glDeleteShader(shader);
        }

        unsigned int GetShader() const { return shader; }
    };
}