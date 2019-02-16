//
// Created by Göksu Güvendiren on 2019-01-20.
//

#pragma once
#include <vector>

#include <glm/glm.hpp>

#include "vertex.hpp"

namespace grt
{
    class mesh
    {
        unsigned int vao;
        unsigned int vbo;
        unsigned int ebo;

        std::vector<grt::vertex> vertices;
        std::vector<unsigned int> indices;

        glm::mat4 transformation;

    public:
        mesh(std::vector<grt::vertex> vert, std::vector<unsigned int> ind, bool center = false);
        mesh(std::vector<float> vert);

        std::pair<glm::vec3, glm::vec3> FindMinMax() const;
        glm::vec3 FindMiddle() const;
        void Draw();
    };
}