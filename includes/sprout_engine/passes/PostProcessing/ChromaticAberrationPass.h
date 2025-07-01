//
// Created by arthu on 26/12/2024.
//

#pragma once


#include "postProcessPass.h"

class ChromaticAberrationPass : public PostProcessPass
{
protected:
    float strength{1.f};
    bool fadeFromCenter{true};

public:
    ChromaticAberrationPass(int width, int height, const char* vertShader, const char* fragShader);
    void render(Pass* input) override;
    void drawInspector(Camera *camera) override;
};
