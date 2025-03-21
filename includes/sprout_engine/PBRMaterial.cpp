//
// Created by Bellaedris on 15/03/2025.
//

#include "PBRMaterial.h"

#include <utility>
#include "resourcesManager.h"

PBRMaterial::PBRMaterial(
        const std::string &path,
        const std::string &name,
        const std::array<std::string, 5> &texturePaths
)
    : m_name(std::move(name))
{
    for(int i = 0; i < texturePaths.size(); i++)
    {
        m_materialTextures[i] = ResourcesManager::GetInstance()->cacheTexture(
                path,
                texturePaths[i],
                i == materialTypes::DIFFUSE
        );
    }
}

TexturePtr PBRMaterial::GetTexture(int index)
{
    return m_materialTextures[index];
}
