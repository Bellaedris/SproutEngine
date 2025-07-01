//
// Created by arthu on 26/12/2024.
//

#include "filmGrainPass.h"

FilmGrainPass::FilmGrainPass(int width, int height, const char* vertShader, const char* fragShader)
    : PostProcessPass(width, height, vertShader, fragShader)
{
}

void FilmGrainPass::render(Pass* input) {
    shader.use();
    shader.uniform_data("strength", strength);
    input->activateTexture();
    shader.uniform_data("framebuffer", 0);

    bindAndDrawEffect();
}

void FilmGrainPass::drawInspector(Camera *camera)
{
    if(ImGui::TreeNode("Film Grain"))
    {
        ImGui::Checkbox("Active", &isActive);
        ImGui::SliderFloat("Strength", &strength, 0.f, 1.f);

        ImGui::TreePop();
    }
}