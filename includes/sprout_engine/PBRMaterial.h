//
// Created by Bellaedris on 15/03/2025.
//

#pragma once


#include <array>
#include "texture.h"

class PBRMaterial
{
private:
    std::array<TexturePtr, 5> m_materialTextures{};
    //std::array<glm::vec3, 5> m_materialColors{};
    std::string m_name{};

public:
    PBRMaterial(std::string name, const std::array<std::string, 5>& texturePaths);

    TexturePtr GetTexture(int index);

    enum materialTypes
    {
        DIFFUSE,
        NORMAL,
        METALROUGHNESS,
        AO,
        EMISSIVE
        //ALPHA
    };

    static const int TEXTURE_CHANNELS = 5;
};

using PBRMaterialPtr = std::shared_ptr<PBRMaterial>;