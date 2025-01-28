//
// Created by arthu on 26/12/2024.
//

#include "filmGrainPass.h"

FilmGrainPass::FilmGrainPass(int width, int height, const char* vertShader, const char* fragShader)
    : PostProcessPass(width, height, vertShader, fragShader)
{
    glGenFramebuffers(1, &m_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

    m_texture = Texture(width, height, GL_RGBA32F, GL_RGBA, GL_FLOAT);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture.handle(), 0);

    m_renderQuad = Mesh::generatePlane();
}

void FilmGrainPass::render(Pass* input) {
    shader.use();
    shader.uniform_data("strength", strength);
    input->activateTexture();
    shader.uniform_data("frame", 0);

    if(m_bIsFinal)
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    else
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_framebuffer);

    glDisable(GL_DEPTH_TEST);

    m_renderQuad.draw_strip();

    glEnable(GL_DEPTH_TEST);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    m_bIsFinal = false;
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