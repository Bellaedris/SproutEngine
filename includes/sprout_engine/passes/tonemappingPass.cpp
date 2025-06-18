//
// Created by arthu on 26/12/2024.
//

#include "tonemappingPass.h"

TonemappingPass::TonemappingPass(int width, int height, const char* vertShader, const char* fragShader)
    : PostProcessPass(width, height, vertShader, fragShader)
{
}

void TonemappingPass::render(Pass* input) {
    shader.use();
    shader.uniform_data("exposure", exposure);
    shader.uniform_data("gamma", gamma);
    shader.uniform_data("technique", technique);
    input->activateTexture();
    shader.uniform_data("frame", 0);

    bindAndDrawEffect();
}

const char* tonemappingTechniques[] = {"Reinhardt", "ACES", "Filmic"};

void TonemappingPass::drawInspector(Camera *camera)
{
    if(ImGui::TreeNode("Tonemapping"))
    {
        ImGui::Checkbox("Active", &isActive);
        ImGui::InputFloat("Gamma", &gamma);
        ImGui::InputFloat("Exposure", &exposure);

        const char* comboPreview = tonemappingTechniques[technique];
        if (ImGui::BeginCombo("Tonemapping curve", comboPreview))
        {
            for (int n = 0; n < IM_ARRAYSIZE(tonemappingTechniques); n++)
            {
                const bool is_selected = (technique == n);
                if (ImGui::Selectable(tonemappingTechniques[n], is_selected))
                    technique = n;

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        ImGui::TreePop();
    }
}
