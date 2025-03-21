//
// Created by Bellaedris on 20/03/2025.
//

#pragma once

#include <map>
#include <optional>

#include "PBRMaterial.h"
#include "texture.h"

class ResourcesManager
{
private:
    static ResourcesManager* m_instance;

    std::map<std::string, TexturePtr> m_textureCache {};
    std::map<std::string, PBRMaterialPtr> m_materialCache {};

public:
    ResourcesManager() = default;
    ResourcesManager(const ResourcesManager&) = delete;
    void operator=(const ResourcesManager&) = delete;

    static ResourcesManager* GetInstance();

    TexturePtr cacheTexture(const std::string& texturePath);

    PBRMaterialPtr cacheMaterial(const std::string& name, const std::array<std::string, 5>& texturePaths);
};