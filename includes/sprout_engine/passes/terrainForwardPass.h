//
// Created by Bellaedris on 01/07/2025.
//

#pragma once

#include "drawPass.h"

class TerrainForwardPass : public DrawPass
{
protected:
    Texture m_depthTexture;
public:
    TerrainForwardPass(int width, int height);
    void render(Mesh &terrain);

    Texture& getDepthTexture() { return m_depthTexture; };
    void useDepthTexture(int unit = 0);
    void BuildTextures() override;
};
