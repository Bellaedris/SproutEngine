//
// Created by Bellaedris on 01/07/2025.
//

#include "distanceFogPass.h"

DistanceFogPass::DistanceFogPass(int width, int height, const char *vert, const char *frag, const Texture &depthTexture)
        : PostProcessPass(width, height, vert, frag)
          , depthTexture(depthTexture)
{

}


void DistanceFogPass::render(Pass *input)
{
    shader.use();
    input->activateTexture();
    shader.uniform_data("framebuffer", 0);
    depthTexture.use(1);
    shader.uniform_data("depthTexture", 1);

    shader.uniform_data("maxFogDistance", maxFogDistance);
    shader.uniform_data("fogDensity", fogDensity);
    shader.uniform_data("fogColor", fogColor);

    bindAndDrawEffect();
}

void DistanceFogPass::drawInspector(Camera *camera)
{
    if(ImGui::TreeNode("Distance Fog"))
    {
        ImGui::InputFloat("Max Distance", &maxFogDistance, .1f, 1.f);
        ImGui::InputFloat("Density", &fogDensity, .1f, 1.f);
        ImGui::ColorPicker4("fogColor", glm::value_ptr(fogColor));

        ImGui::TreePop();
    }
}

void DistanceFogPass::sendCameraData(const Camera &cam)
{
    shader.use();
    shader.uniform_data("znear", cam.getZnear());
    shader.uniform_data("zfar", cam.getZfar());

    mat4 inverseProj = glm::inverse(cam.projection());
    shader.uniform_data("inverseProjectionMatrix", inverseProj);
    shader.uniform_data("cameraPos", cam.get_position());
}
