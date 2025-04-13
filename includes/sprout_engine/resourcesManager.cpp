//
// Created by Bellaedris on 20/03/2025.
//

#include "resourcesManager.h"

ResourcesManager* ResourcesManager::m_instance = nullptr;

TexturePtr ResourcesManager::cacheTexture(const std::string &path, const std::string &textureName, bool useSRGB)
{
    // if the path is empty (no texture for this channel), return the default black fallback texture
    if(textureName.empty())
        return m_defaultTexture;

    std::string fullName = path + textureName;
    // cache the texture if it does not exists already
    if(m_textureCache.find(fullName) == m_textureCache.end())
        m_textureCache.emplace(fullName, std::make_shared<Texture>(fullName.c_str(), useSRGB ? "texture_diffuse" : ""));

    // else, return the existing texture
    return m_textureCache[fullName];
}

PBRMaterialPtr ResourcesManager::cacheMaterial(
        const std::string &path,
        const std::string &name,
        const std::array<std::string, 5> &texturePaths
)
{
    auto fullName = path + name;

    // cache the texture if it does not exists already
    if(m_textureCache.find(fullName) == m_textureCache.end())
        m_materialCache.emplace(fullName, std::make_shared<PBRMaterial>(path, name, texturePaths));

    // return the existing texture
    return m_materialCache[fullName];
}

ResourcesManager *ResourcesManager::GetInstance()
{
    if(m_instance == nullptr)
    {
        m_instance = new ResourcesManager();
        m_instance->m_defaultTexture = std::make_unique<Texture>(Texture::buildDefaultTexture());
    }

    return m_instance;
}

std::optional<PBRMaterialPtr> ResourcesManager::getMaterial(const std::string &path, const std::string &name)
{
    const auto res = m_materialCache.find(path + name);

    if(res != m_materialCache.end())
        return std::make_optional(res->second);
    else
        return {};
}

//MeshPtr ResourcesManager::cacheMesh(const std::string &path, const std::string &meshName)
//{
//    return MeshPtr();
//}
//
//std::optional<MeshPtr> ResourcesManager::getMesh(const std::string &path, const std::string &meshName)
//{
//    const auto res = m_meshCache.find(path + meshName);
//
//    if(res != m_meshCache.end())
//        return std::make_optional(res->second);
//    else
//        return {};
//}
