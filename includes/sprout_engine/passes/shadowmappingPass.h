//
// Created by Bellaedris on 30/03/2025.
//

#pragma once

#include "drawPass.h"
#include "sprout_engine/light.h"

class ShadowmappingPass : public DrawPass
{
protected:
    float m_orthographicZoom{10.f};

public:
    ShadowmappingPass(int width, int height);
    void BuildTextures() override;

    void setResolution(int size);
    void setOrthographicZoom(float zoom);

    void render(const std::unique_ptr<Entity> &rootNode, const DirectionalLight &mainLight, Shader& shader);

    // store the lightspace matrix computed here to be reused in the forward pass. I know, kinda ugly. Still thinking about a better solution
    glm::mat4 m_lightspaceMatrix{};
};
