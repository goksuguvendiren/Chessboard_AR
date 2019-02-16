//
// Created by Göksu Güvendiren on 2019-02-10.
//

#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>

namespace grt
{
    class transform
    {
        glm::mat4 transf;
        glm::vec3 translate = {0, 0, 0.f};
        glm::vec3 scale     = {1.f, 1.f, 1.f};
        glm::quat rotation  = glm::identity<glm::quat>();

        void recalculate()
        {
            auto s = glm::scale(scale);
            auto t = glm::translate(translate);
            auto r = glm::toMat4(rotation);

            transf = t * r * s;
        }

    public:
        transform() : transf(glm::mat4(1.0f)) {}
        transform(const glm::mat4& tr) : transf(tr) {}
        transform(const glm::vec3& tr, const glm::vec3& sc) : translate(tr), scale(sc)
        {}

        void SetPosition(const glm::vec3& pos)
        {
            translate = pos;
            recalculate();
        }

        void Translate(const glm::vec3 &tr)
        {
            translate += tr;
            recalculate();
        }

        void Scale(const glm::vec3 &sc)
        {
            scale = sc;
            recalculate();
        }

        void Rotate(float angle, const glm::vec3& axis)
        {
            auto rot = glm::angleAxis(glm::radians(angle), axis);
            rotation *= rot;
            rotation = glm::normalize(rotation);

            recalculate();
        }

        glm::mat4 Data() const { return transf; }
        glm::vec3 GetPosition() const { return glm::vec3{ transf * glm::vec4{0, 0, 0, 1} }; }
    };


}