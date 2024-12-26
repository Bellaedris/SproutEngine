//
// Created by arthu on 26/12/2024.
//

#pragma once


#include "pass.h"

class TonemappingPass : public Pass
{
protected:
    Mesh m_renderQuad;
public:
    TonemappingPass(int width, int height);
    void render(std::vector<Entity> &entities, const Camera &camera, Shader &shader) override;
};
