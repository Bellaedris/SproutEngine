//
// Created by Bellaedris on 06/11/2024.
//

#include <iostream>
#include "gpuMeshReader.h"
#include "sprout_engine/color.h"

GpuMesh::GpuMesh(const std::string &path)
{
    Assimp::Importer importer;
    unsigned int flags = aiProcessPreset_TargetRealtime_Fast | aiProcess_GenBoundingBoxes;

    auto scene = importer.ReadFile(path, flags);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "error assimp: " << importer.GetErrorString() << std::endl;
        return;
    }

    // used for texture parsing, but we don't want any for now
    //directory = path.substr(0, path.find_last_of('/'));

    processNode(scene->mRootNode, scene);
    processMaterials(scene);
}

void GpuMesh::processNode(const aiNode *node, const aiScene *scene)
{
    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        processMesh(scene->mMeshes[node->mMeshes[i]], scene);
    }

    for(unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

void GpuMesh::processMesh(const aiMesh *mesh, const aiScene *scene)
{
    for(unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        const auto va = mesh->mVertices[mesh->mFaces[i].mIndices[0]];
        const auto vb = mesh->mVertices[mesh->mFaces[i].mIndices[1]];
        const auto vc = mesh->mVertices[mesh->mFaces[i].mIndices[2]];
        const auto na = mesh->mNormals[mesh->mFaces[i].mIndices[0]];
        const auto nb = mesh->mNormals[mesh->mFaces[i].mIndices[1]];
        const auto nc = mesh->mNormals[mesh->mFaces[i].mIndices[2]];
        m_triangles.push_back
        ({
                 {va.x, va.y, va.z, 1.f},
                 {vb.x, vb.y, vb.z, 1.f},
                 {vc.x, vc.y, vc.z, 1.f},
                 {na.x, na.y, na.z, 1.f},
                 {nb.x, nb.y, nb.z, 1.f},
                 {nc.x, nc.y, nc.z, 1.f},
                 mesh->mMaterialIndex
        });
    }
}

void GpuMesh::processMaterials(const aiScene *scene)
{
    for(unsigned int i = 0; i < scene->mNumMaterials; i++)
    {
        auto mat = scene->mMaterials[i];

        Color diffuse, emissive;

        aiColor3D col(0.f, 0.f, 0.f);
        if(AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_DIFFUSE, col))
            diffuse = Color(col.r, col.g, col.b, 1.f);

        col = aiColor3D(0.f, 0.f, 0.f);
        if(AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_EMISSIVE, col))
            emissive = Color(col.r, col.g, col.b, 1.f);

        m_materials.push_back
        ({
            diffuse,
            emissive
        });
    }
}

std::vector<TriangleData> &GpuMesh::GetTriangleData()
{
    return m_triangles;
}

std::vector<MaterialData> &GpuMesh::GetMaterialData()
{
    return m_materials;
}
