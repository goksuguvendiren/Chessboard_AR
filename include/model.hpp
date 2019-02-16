//
// Created by Göksu Güvendiren on 2019-01-20.
//

#pragma once

#include <vector>
#include <mesh.hpp>
#include <transform.hpp>

#include <glm/glm.hpp>

#include <assimp/scene.h>

namespace grt
{
    class model
    {
        std::vector<grt::mesh> meshes;
        void processNode(aiNode* node, const aiScene* scene);

        grt::transform transf;
        glm::vec3      albedo;

    public:
        explicit model(const std::string& path);
        explicit model(const grt::mesh& mesh);
        void Draw();
        void Apply(const grt::transform& tr)
        {
            transf = tr;
        }

        grt::transform GetTransform() const { return transf; }
        glm::vec3      GetColor() const { return albedo; }
        void           SetColor(const glm::vec3& col) { albedo = col; }
    };
}