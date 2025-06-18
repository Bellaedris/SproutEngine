//
// Created by arthu on 26/12/2024.
//

#include "ChromaticAberrationPass.h"

ChromaticAberrationPass::ChromaticAberrationPass(int width, int height, const char* vertShader, const char* fragShader)
    : PostProcessPass(width, height, vertShader, fragShader)
{
}

void ChromaticAberrationPass::render(Pass* input) {
    shader.use();
    shader.uniform_data("strength", strength);
    shader.uniform_data("fadeFromCenter", fadeFromCenter);
    input->activateTexture();
    shader.uniform_data("frame", 0);

    bindAndDrawEffect();
}

void ChromaticAberrationPass::drawInspector(Camera *camera)
{
    if(ImGui::TreeNode("Chromatic Aberration"))
    {
        ImGui::Checkbox("Active", &isActive);
        ImGui::InputFloat("Strength", &strength);
        ImGui::Checkbox("Fade from center", &fadeFromCenter);

        ImGui::TreePop();
    }
}