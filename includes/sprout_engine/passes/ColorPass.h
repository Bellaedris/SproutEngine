//
// Created by Bellaedris on 26/12/2024.
//

#pragma once


#include "drawPass.h"

class ColorPass : public DrawPass
{
protected:
    Texture m_depthTexture;
public:
    ColorPass(int width, int height);
    void render(std::vector<Entity> &entities, const Camera &camera, Shader &shader) override;
};
