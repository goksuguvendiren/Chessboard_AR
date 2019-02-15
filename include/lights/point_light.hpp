//
// Created by Göksu Güvendiren on 2019-01-21.
//

#pragma once

#include <glm/vec3.hpp>

namespace grt
{
    class point_light
    {
        glm::vec3 intensity;

    public:
        point_light(const glm::vec3& ints) : intensity(ints) {}

        const glm::vec3& GetIntensity() const { return intensity; }
    };
}