//
// Created by arthu on 26/12/2024.
//

#pragma once


#include "postProcessPass.h"

class FilmGrainPass : public PostProcessPass
{
protected:
    float strength{.25f};

public:
    FilmGrainPass(int width, int height, const char* vertShader, const char* fragShader);
    void render(Pass* input) override;
    void drawInspector(Camera *camera) override;
};
