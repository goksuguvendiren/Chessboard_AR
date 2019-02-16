//
// Created by Göksu Güvendiren on 2019-01-20.
//

#include <iostream>

#include <model.hpp>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/transform.hpp>

static void check_errors(const Assimp::Importer& importer, const aiScene* scene)
{
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "Error::Assimp::" << importer.GetErrorString() << '\n';
    }
}

static glm::vec3 CalculateNormal(const aiFace& face, const std::vector<grt::vertex>& vertices)
{
    auto a = vertices[face.mIndices[0]].Position();
    auto b = vertices[face.mIndices[1]].Position();
    auto c = vertices[face.mIndices[2]].Position();

    auto ab = b - a;
    auto ac = c - a;

    return glm::normalize(glm::cross(ab, ac));
}

grt::mesh processMesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<grt::vertex> vertices;
    std::vector<unsigned int> indices;
    // std::vector<Texture> textures;

    for (auto i = 0u; i < mesh->mNumVertices; ++i)
    {
        auto vert = mesh->mVertices[i];

        grt::vertex vertex({vert.x, vert.y, vert.z});
        vertices.push_back(vertex);
    }

    for (auto i = 0u; i < mesh->mNumFaces; ++i)
    {
        auto face = mesh->mFaces[i];

        assert(face.mNumIndices == 3 && "Mesh should be triangles.");
        indices.push_back(face.mIndices[0]);
        indices.push_back(face.mIndices[1]);
        indices.push_back(face.mIndices[2]);

        auto normal = CalculateNormal(face, vertices);

        vertices[face.mIndices[0]].SetNormal(normal);
        vertices[face.mIndices[1]].SetNormal(normal);
        vertices[face.mIndices[2]].SetNormal(normal);
    }

    return grt::mesh(std::move(vertices), std::move(indices), true);
}

void grt::model::processNode(aiNode* node, const aiScene* scene)
{
    for (auto i = 0u; i < node->mNumMeshes; ++i)
    {
        auto mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }

    for (auto i = 0u; i < node->mNumChildren; ++i)
    {
        processNode(node->mChildren[i], scene);
    }
}

grt::model::model(const std::string& path)
{
    Assimp::Importer importer;
    auto sc = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);
    check_errors(importer, sc);

    auto dir = path.substr(0, path.find_last_of('/'));
    processNode(sc->mRootNode, sc);

    albedo = {40, 20, 60};
}

void grt::model::Draw()
{
    for (auto& mesh : meshes)
    {
        mesh.Draw();
    }
}

grt::model::model(const grt::mesh& mesh)
{
    meshes.push_back(mesh);
    albedo = {40, 20, 60};
}
