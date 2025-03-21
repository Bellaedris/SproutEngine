//
// Created by Bellaedris on 15/03/2025.
//

#include "PBRMaterial.h"

#include <utility>
#include "resourcesManager.h"

PBRMaterial::PBRMaterial(std::string name, const std::array<std::string, 5>& texturePaths)
    : m_name(std::move(name))
{
    for(int i = 0; i < texturePaths.size(); i++)
    {
        if(!texturePaths[i].empty())
            m_materialTextures[i] = ResourcesManager::GetInstance()->cacheTexture(texturePaths[i]);
    }
}

TexturePtr PBRMaterial::GetTexture(int index)
{
    return m_materialTextures[index];
}
