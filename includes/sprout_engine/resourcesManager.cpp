//
// Created by Bellaedris on 20/03/2025.
//

#include "resourcesManager.h"

ResourcesManager* ResourcesManager::m_instance = nullptr;

TexturePtr ResourcesManager::cacheTexture(const std::string &texturePath)
{
    // cache the texture if it does not exists already
    if(m_textureCache.find(texturePath) == m_textureCache.end())
        m_textureCache.emplace(texturePath, std::make_shared<Texture>(texturePath.c_str()));

    // else, return the existing texture
    return m_textureCache[texturePath];
}

PBRMaterialPtr ResourcesManager::cacheMaterial(const std::string &name, const std::array<std::string, 5> &texturePaths)
{
    // cache the texture if it does not exists already
    if(m_textureCache.find(name) == m_textureCache.end())
        m_materialCache.emplace(name, std::make_shared<PBRMaterial>(name, texturePaths));

    // return the existing texture
    return m_materialCache[name];
}

ResourcesManager *ResourcesManager::GetInstance()
{
    if(m_instance == nullptr)
        m_instance = new ResourcesManager();

    return m_instance;
}