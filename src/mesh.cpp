//
// Created by Göksu Güvendiren on 2019-01-20.
//

#include <mesh.hpp>
#include <glad/glad.h>

#include <iostream>

#include <algorithm>

grt::mesh::mesh(std::vector<grt::vertex> vert, std::vector<unsigned int> ind, bool center) : vertices(std::move(vert)), indices(std::move(ind)),
                                                                                transformation(glm::mat4(1.0f))
{
    if (center)
    {
        auto middle = FindMiddle();
        std::cerr << middle.x << '\n';
        std::cerr << middle.y << '\n';
        std::cerr << middle.z << '\n';
        for (auto& vertex : vertices)
        {
            auto new_position = vertex.Position() - middle;
            vertex.SetPosition(new_position);
        }

        middle = FindMiddle();
    }

    glGenBuffers(1, &ebo);
    glGenBuffers(1, &vbo);
    glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(grt::vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(grt::vertex), (void*)0);

    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(grt::vertex), (void*)offsetof(grt::vertex_data, normal));

    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(grt::vertex), (void*)offsetof(grt::vertex_data, texcoord));

    glBindVertexArray(0);
}
grt::mesh::mesh(std::vector<float> vert) : vertices(), indices(), transformation(glm::mat4(1.0f))
{
//    auto middle = FindMiddle();
//    std::cerr << middle.x << '\n';
//    std::cerr << middle.y << '\n';
//    std::cerr << middle.z << '\n';
//    for (auto& vertex : vertices)
//    {
//        auto new_position = vertex.Position() - middle;
//        vertex.SetPosition(new_position);
//    }
//
//    middle = FindMiddle();

//    glGenBuffers(1, &ebo);
    glGenBuffers(1, &vbo);
    glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, /*vertices.size() * */vert.size(), &vertices[0], GL_STATIC_DRAW);

//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0);

    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float), (void*)(3 * sizeof(float)));

    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float), (void*)(6 * sizeof(float)));

    glBindVertexArray(0);
}

void grt::mesh::Draw()
{
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

std::pair<glm::vec3, glm::vec3> grt::mesh::FindMinMax() const
{
    auto x_minmax = std::minmax_element(vertices.begin(), vertices.end(), [](const grt::vertex &v1, const grt::vertex &v2) {
        return v1.Position().x < v2.Position().x;
    });

    auto y_minmax = std::minmax_element(vertices.begin(), vertices.end(), [](const grt::vertex &v1, const grt::vertex &v2) {
        return v1.Position().y < v2.Position().y;
    });

    auto z_minmax = std::minmax_element(vertices.begin(), vertices.end(), [](const grt::vertex &v1, const grt::vertex &v2) {
        return v1.Position().z < v2.Position().z;
    });

    glm::vec3 min_values = {x_minmax.first->Position().x, y_minmax.first->Position().y, z_minmax.first->Position().z};
    glm::vec3 max_values = {x_minmax.second->Position().x, y_minmax.second->Position().y, z_minmax.second->Position().z};

    return {min_values, max_values};
}

glm::vec3 grt::mesh::FindMiddle() const
{
    auto min_max = FindMinMax();

    return {(min_max.first.x + min_max.second.x) / 2.f, (min_max.first.y + min_max.second.y) / 2.f, (min_max.first.z + min_max.second.z) / 2.f};
}
