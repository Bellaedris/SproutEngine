//
// Created by Bellaedris on 26/12/2024.
//

#pragma once


#include "drawPass.h"

class ForwardPass : public DrawPass
{
protected:
    Texture m_depthTexture;
public:
    ForwardPass(int width, int height);
    void render(
            std::vector<Entity> &entities,
            const Camera &camera,
            const glm::mat4 &lightspaceMatrix,
            Shader &shader
    );
};
