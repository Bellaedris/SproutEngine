//
// Created by Bellaedris on 20/03/2025.
//

#pragma once

#include <map>
#include <optional>

#include "PBRMaterial.h"
#include "texture.h"
#include "mesh.h"

class ResourcesManager
{
private:
    static ResourcesManager* m_instance;

    // Cache of all loaded resources
    std::map<std::string, TexturePtr> m_textureCache {};
    std::map<std::string, PBRMaterialPtr> m_materialCache {};
    //std::map<std::string, MeshPtr> m_meshCache {}; still a WIP¨, not sure if it should be here

    // default texture used as a fallback option when nothing is provided
    TexturePtr m_defaultTexture;

public:
    ResourcesManager() = default;
    ResourcesManager(const ResourcesManager&) = delete;
    void operator=(const ResourcesManager&) = delete;

    static ResourcesManager* GetInstance();

    TexturePtr cacheTexture(const std::string &path, const std::string &textureName, bool useSRGB);

    PBRMaterialPtr cacheMaterial(const std::string &path, const std::string &name, const std::array<std::string, 5> &texturePaths);
    std::optional<PBRMaterialPtr> getMaterial(const std::string &path, const std::string &name);

//    MeshPtr cacheMesh(const std::string &path, const std::string &meshName);
//    std::optional<MeshPtr> getMesh(const std::string &path, const std::string &meshName);
};