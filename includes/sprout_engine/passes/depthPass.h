//
// Created by Bellaedris on 23/12/2024.
//

#include "sprout_engine/texture.h"
#include "pass.h"

class DepthPass : public Pass
{
protected:
    float m_orthographicZoom;


public:
    DepthPass(int width, int height);

    void render(std::vector<Entity> &entities, const Camera &camera, Shader& shader) override;
};