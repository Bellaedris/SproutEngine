//
// Created by Bellaedris on 01/07/2025.
//

#pragma once

#include "postProcessPass.h"

class DistanceFogPass : public PostProcessPass
{
protected:
    Texture depthTexture;
    float maxFogDistance{50.f};
    float fogDensity{.5f};
    Color fogColor{.7f, .7f, .7f, 1.f};

public:
    DistanceFogPass(int width, int height, const char* vert, const char* frag, const Texture& depthTexture);

    void sendCameraData(const Camera& cam) override;

    void render(Pass* input) override;
    void drawInspector(Camera *camera) override;
};
