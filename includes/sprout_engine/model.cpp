//
// Created by Bellaedris on 28/04/2024.
//

#include "model.h"

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

Mesh Model::processMesh(const aiMesh *mesh, const aiScene *scene) {
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texcoords;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    // process all vertex data
    for (int i = 0; i < mesh->mNumVertices; i++)
    {
        positions.emplace_back(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        normals.emplace_back(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);

        if (mesh->mTextureCoords[0])
            texcoords.emplace_back(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        else
            texcoords.emplace_back(0.f, 0.f);
    }

    //process indices
    for (int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    // process textures
    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<Texture> diffuse_maps = loadMaterialTexture(mat, aiTextureType_DIFFUSE, "texture_diffuse");
        std::vector<Texture> specular_maps = loadMaterialTexture(mat, aiTextureType_SPECULAR, "texture_specular");

        textures.insert(textures.end(), diffuse_maps.begin(), diffuse_maps.end());
        textures.insert(textures.end(), specular_maps.begin(), specular_maps.end());
    }

    return {positions, normals, texcoords, indices, textures, mesh->mAABB};
}

std::vector<Texture> Model::loadMaterialTexture(const aiMaterial *mat, aiTextureType type, std::string type_name) {
    std::vector<Texture> textures;
    aiString str;

    #pragma omp parallel for
    for (int i = 0; i < mat->GetTextureCount(type); i++)
    {
        mat->GetTexture(type, i, &str);
        std::string path = directory + "/" + str.C_Str();

        bool skip = false;
        for (const Texture& t : loaded_textures)
        {
            if (strcmp(t.path.c_str(), path.c_str()) == 0)
            {
                textures.push_back(t);
                skip = true;
                break;
            }
        }

        if (!skip) {
            Texture texture(path.c_str(), type_name);
            textures.push_back(texture);
            loaded_textures.push_back(texture);
        }
    }

    return textures;
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

    directory = path.substr(0, path.find_last_of('/'));

    processNode(scene->mRootNode, scene);
}

void Model::draw(Shader &s, const Frustum &f, const Transform &transform) {
    for (Mesh& m : meshes)
    {
        m.draw(s, f, transform);
    }
}

void Model::draw(Shader &s) {
    for (Mesh& m : meshes)
    {
        m.draw(s);
    }
}

void Model::drawAABB(Shader &s, const mat3 &transform, const vec3 &translation) {
    for (Mesh& m : meshes)
    {
        m.drawAABB(s, transform, translation);
    }
}

void Model::draw_debug_frustum(Shader &s, const Frustum &frustum, const Transform &transform) {
    for (Mesh& m : meshes)
    {
        m.draw_debug_frustum(s, frustum, transform);
    }
}
