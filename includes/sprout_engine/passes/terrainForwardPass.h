//
// Created by Bellaedris on 01/07/2025.
//

#pragma once

#include "drawPass.h"

class TerrainForwardPass : public DrawPass
{
protected:
    Texture m_depthTexture;
    Shader skyboxShader;
    Mesh skyboxCube;
    glm::vec3 sunColor {1.f, 0.f, 0.f};
    glm::vec3 sunDir {1.f, 0.f, -5.f};
    float sunSize{.05f};
public:
    TerrainForwardPass(int width, int height, const char* vertShader, const char* fragShader);
    void render(Mesh &terrain, Camera* camera);

    void setSunColor(const glm::vec3& color) { sunColor = color; };
    void setSunDir(const glm::vec3& dir) { sunDir = dir; };
    void setSunSize(float size) { sunSize = size; };
    Texture& getDepthTexture() { return m_depthTexture; };
    void useDepthTexture(int unit = 0);
    void BuildTextures() override;
};
