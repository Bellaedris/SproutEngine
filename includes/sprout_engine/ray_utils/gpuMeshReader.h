//
// Created by Bellaedris on 06/11/2024.
//

#pragma once

#include <string>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <vector>

struct TriangleData
{
    glm::vec4 a;
    glm::vec4 b;
    glm::vec4 c;
    glm::vec4 na;
    glm::vec4 nb;
    glm::vec4 nc;
    unsigned int materialId;
};

struct DirLightData
{
    glm::vec4 ambiant;
    glm::vec4 diffuse;
    glm::vec4 specular;
    glm::vec4 dir;
};

struct MaterialData
{
    glm::vec4 diffuse;
    glm::vec4 emissive;
};

class GpuMesh
{
public:
    GpuMesh() = default;
    explicit GpuMesh(const std::string& path);

    std::vector<TriangleData>& GetTriangleData();
    std::vector<MaterialData>& GetMaterialData();
protected:
    std::vector<TriangleData> m_triangles;
    std::vector<MaterialData> m_materials;

    void processNode(const aiNode *node, const aiScene *scene);
    void processMesh(const aiMesh* mesh, const aiScene* scene);
    void processMaterials(const aiScene* scene);
};
