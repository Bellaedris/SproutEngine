//
// Created by arthu on 26/12/2024.
//

#pragma once

#include "postProcessPass.h"

class TonemappingPass : public PostProcessPass
{
protected:
    float gamma{2.2f};
    float exposure{1.f};
    int technique{1};

public:
    TonemappingPass(int width, int height, const char* vertShader, const char* fragShader);
    void render(Pass* input) override;
    void drawInspector(Camera *camera) override;
};
