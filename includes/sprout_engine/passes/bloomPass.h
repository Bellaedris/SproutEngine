//
// Created by Bellaedris on 18/06/2025.
//

#pragma once


#include "postProcessPass.h"

class BloomPass : public PostProcessPass
{
protected:
    float threshold {1.f};
    const float BRIGHT_PASS_RESOLUTION_MULTIPLIER = .5f;

    // bright pass data
    int brightPassWidth;
    int brightPassHeight;
    Texture bloomPassTextures[2];
    GLuint bloomPassFramebuffers[2];
    Shader brightPassShader;

    // gaussian blur data
    Texture blurPass;
    Shader gaussianShader;

public:
    BloomPass(int p_width, int p_height, const char *p_vertShader, const char *p_fragShader, const char *p_brightVertShader, const char *p_brightFragShader, const char *gaussianFragShader);

    void render(Pass* input) override;
    void drawInspector(Camera *camera) override;
};
