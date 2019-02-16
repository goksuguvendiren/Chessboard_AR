//
// Created by Göksu Güvendiren on 2019-01-20.
//

#pragma once

#include <glm/glm.hpp>
#include <iostream>

namespace grt
{
    struct vertex_data
    {
        glm::vec3 position;
        glm::vec3 normal{};
        glm::vec2 texcoord = {0.2, 0.2};
    };

    class vertex
    {
        vertex_data data;
        int num_faces = 0;

    public:
        vertex(const glm::vec3& pos, const glm::vec3& norm = {1.0f, 1.0f, 1.0f})
        {
            data.position = pos;
            data.normal = norm;
        }

        glm::vec3 Position() const { return data.position; }
        void SetPosition(const glm::vec3& new_pos) { data.position = new_pos; }

        void SetNormal(const glm::vec3& normal)
        {
            auto current_sum = data.normal * float(num_faces);
            num_faces++;
            data.normal = (current_sum + normal) / float(num_faces);
        }
    };
}