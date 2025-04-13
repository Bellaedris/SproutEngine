//
// Created by Bellaedris on 28/04/2024.
//

#include <map>
#include "model.h"
#include "PBRMaterial.h"
#include "resourcesManager.h"

void Model::processNode(const aiNode *node, const aiScene *scene) {

    for (int i = 0; i < node->mNumMeshes; i++)
    {
        meshes.push_back(
                processMesh(scene->mMeshes[node->mMeshes[i]], scene)
        );
    }
    for (int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

MeshPtr Model::processMesh(const aiMesh *mesh, const aiScene *scene)
{
    std::string path = directory + "/";

    // check if the mesh is cached before anything
//    auto cachedMesh = ResourcesManager::GetInstance()->getMesh(path, mesh->mName.C_Str());
//    if(cachedMesh.has_value())
//    {
//        std::cout << "Skipped cached mesh" << std::endl;
//        return cachedMesh.value();
//    }

    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texcoords;
    std::vector<glm::vec4> colors;
    std::vector<glm::vec3> tangents;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;
    Material material;

    // process all vertex data
    for (int i = 0; i < mesh->mNumVertices; i++)
    {
        positions.emplace_back(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        normals.emplace_back(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);

        if (mesh->mColors[0])
            colors.emplace_back(mesh->mColors[i]->r, mesh->mColors[i]->g, mesh->mColors[i]->b, mesh->mColors[i]->a);

        if (mesh->mTextureCoords[0])
            texcoords.emplace_back(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        else
            texcoords.emplace_back(0.f, 0.f);
    }

    // process Tangents, if necessary
    if(mesh->HasTangentsAndBitangents())
        for(int i = 0; i < mesh->mNumVertices; i++)
            tangents.emplace_back(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);

    //process indices
    for (int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    PBRMaterialPtr meshMaterial;
    // process textures
    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];

        aiString name;
        mat->Get(AI_MATKEY_NAME, name);
        // if the mat is not named, use the index instead
        if(strcmp(name.C_Str(), "") == 0)
            name = aiString("material" + std::to_string(mesh->mMaterialIndex));

        const auto cached = ResourcesManager::GetInstance()->getMaterial(path, name.C_Str());
        if(cached.has_value())
        {
            std::cout << "skipped cached material" << std::endl;
            return std::make_shared<Mesh>(positions, normals, texcoords, tangents, colors, indices, cached.value(), mesh->mAABB);
        }

        // try to read each PBRMaterial texture. If it has none, store either the material info or a default value
        aiString diffusePath;
        mat->GetTexture(aiTextureType_DIFFUSE, 0, &diffusePath);

        aiString normalPath;
        mat->GetTexture(aiTextureType_NORMALS, 0, &normalPath);

        aiString metalroughnessPath;
        mat->GetTexture(aiTextureType_UNKNOWN, 0, &metalroughnessPath); // metalRough are still packed in unknown...

        aiString aoPath;
        mat->GetTexture(aiTextureType_LIGHTMAP, 0, &aoPath);

        aiString emissivePath;
        mat->GetTexture(aiTextureType_EMISSIVE, 0, &emissivePath);

        std::array<std::string, 5> materialTextures = {
                diffusePath.C_Str(),
                normalPath.C_Str(),
                metalroughnessPath.C_Str(),
                aoPath.C_Str(),
                emissivePath.C_Str()
        };
        meshMaterial = ResourcesManager::GetInstance()->cacheMaterial(path, name.C_Str(), materialTextures);

        // old workflow that should be deleted and readapted to keep the raytracer functional...
        //if there is no diffuse texture, store a diffuse color
//        if (mat->GetTextureCount(aiTextureType_DIFFUSE) < 1) {
//            aiColor4D color;
//            if (AI_SUCCESS == aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, &color)) {
//                material.diffuse = Color(color.r, color.g, color.b, color.a);
//            }
//        }
//        else
//        {
//            std::vector<Texture> diffuse_maps = loadMaterialTexture(mat, aiTextureType_DIFFUSE, "texture_diffuse");
//            std::vector<Texture> normal_maps = loadMaterialTexture(mat, aiTextureType_NORMALS, "texture_normals");
//            std::vector<Texture> ao_maps = loadMaterialTexture(mat, aiTextureType_LIGHTMAP, "texture_ao");
//            std::vector<Texture> metalness_maps = loadMaterialTexture(mat, aiTextureType_METALNESS, "texture_roughnessMetalness");
//            std::vector<Texture> roughness_maps = loadMaterialTexture(mat, aiTextureType_UNKNOWN, "texture_roughnessMetalness");
//            std::vector<Texture> specular_maps = loadMaterialTexture(mat, aiTextureType_SPECULAR, "texture_specular");
//            std::vector<Texture> emissive_maps = loadMaterialTexture(mat, aiTextureType_EMISSIVE, "texture_emissive");
//
//            textures.insert(textures.end(), diffuse_maps.begin(), diffuse_maps.end());
//            textures.insert(textures.end(), specular_maps.begin(), specular_maps.end());
//            textures.insert(textures.end(), normal_maps.begin(), normal_maps.end());
//            textures.insert(textures.end(), ao_maps.begin(), ao_maps.end());
//            textures.insert(textures.end(), metalness_maps.begin(), metalness_maps.end());
//            textures.insert(textures.end(), emissive_maps.begin(), emissive_maps.end());
//        }

    }

    return std::make_shared<Mesh>(positions, normals, texcoords, tangents, colors, indices, meshMaterial, mesh->mAABB);
}

Model::Model(const std::string& path, bool flip_uv) {
    Assimp::Importer importer;
    unsigned int flags = aiProcessPreset_TargetRealtime_Fast | aiProcess_GenBoundingBoxes;
    if (flip_uv)
        flags |= aiProcess_FlipUVs;
    auto scene = importer.ReadFile(path, flags);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "error assimp: " << importer.GetErrorString() << std::endl;
        return;
    }

    directory = path.substr(0, path.find_last_of("/\\"));

    processNode(scene->mRootNode, scene);
}

void Model::draw(Shader &s, const Frustum &f, const Transform &transform) {
    for (MeshPtr& m : meshes)
    {
        m->draw(s, f, transform);
    }
}

void Model::draw(Shader &s) {
    for (MeshPtr& m : meshes)
    {
        m->draw(s);
    }
}

void Model::drawAABB(Shader &s, const mat3 &transform, const vec3 &translation) {
    for (MeshPtr& m : meshes)
    {
        //m.drawAABB(s, transform, translation);
    }
}

void Model::draw_debug_frustum(Shader &s, const Frustum &frustum, const Transform &transform) {
    for (MeshPtr& m : meshes)
    {
        m->draw_debug_frustum(s, frustum, transform);
    }
}
