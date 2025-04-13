//
// Created by Bellaedris on 23/12/2024.
//

#pragma once

#include "sprout_engine/entity.h"

#include "pass.h"

class DrawPass : public Pass
{
public:
    DrawPass(int width, int height) : Pass(width, height) {};
    //virtual void render(std::vector<Entity> &entities, const Camera &camera, Shader &shader) = 0;
};