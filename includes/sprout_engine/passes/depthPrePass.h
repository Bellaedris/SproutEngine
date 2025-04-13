//
// Created by Bellaedris on 23/12/2024.
//

#include "sprout_engine/texture.h"
#include "drawPass.h"

class DepthPrePass : public DrawPass
{
protected:
    float m_orthographicZoom;

public:
    DepthPrePass(int width, int height);

    void render(std::vector<Entity> &entities, const Camera &camera, Shader& shader);
};