//
// Created by Bellaedris on 26/12/2024.
//

#pragma once


#include "pass.h"

class ColorPass : public Pass
{
protected:
    Texture m_depthTexture;
public:
    ColorPass(int width, int height);
    void render(std::vector<Entity> &entities, const Camera &camera, Shader &shader) override;
};
