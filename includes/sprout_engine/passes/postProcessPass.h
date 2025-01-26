//
// Created by Bellaedris on 23/12/2024.
//

#pragma once

#include "pass.h"

class PostProcessPass : public Pass, public Inspectable
{
protected:
    Shader shader;
public:
    bool isActive;

    PostProcessPass(int width, int height, const char* vertShader, const char* fragShader) : Pass(width, height), shader(vertShader, fragShader) {};
    virtual void render(Pass* input) = 0;
    void drawInspector(Camera *camera) override = 0;
};