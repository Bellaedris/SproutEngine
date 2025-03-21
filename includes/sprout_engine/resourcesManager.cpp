//
// Created by Bellaedris on 20/03/2025.
//

#include "resourcesManager.h"

ResourcesManager* ResourcesManager::m_instance = nullptr;

TexturePtr ResourcesManager::cacheTexture(const std::string &path, const std::string &textureName, bool useSRGB)
{
    // if the path is empty (no texture for this channel), return the default fallback texture
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
    // cache the texture if it does not exists already
    if(m_textureCache.find(name) == m_textureCache.end())
        m_materialCache.emplace(name, std::make_shared<PBRMaterial>(path, name, texturePaths));

    // return the existing texture
    return m_materialCache[name];
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