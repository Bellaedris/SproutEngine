//
// Created by Bellaedris on 18/06/2025.
//

#pragma once


#include "postProcessPass.h"
#include "sprout_engine/computeShader.h"

class BloomPass : public PostProcessPass
{
protected:
    float threshold {1.f};
    float intensity {1.f};
    const float BRIGHT_PASS_RESOLUTION_MULTIPLIER = .5f;
    const int DOWNSAMPLING_STEPS = 8;


    // bright pass data
    int brightPassWidth;
    int brightPassHeight;
    Texture bloomPassTextures[2];
    GLuint bloomPassFramebuffers[2];

    // gaussian blur data
    Texture blurPass;

    ComputeShader thresholdingCompute;
    ComputeShader downsampleCompute;
    ComputeShader upsampleCompute;
    Texture thresholdTexture;
    std::vector<Texture> mipChain;

public:
    BloomPass(int p_width, int p_height, const char *p_vertShader, const char *p_fragShader);

    void render(Pass* input) override;
    void drawInspector(Camera *camera) override;
};
