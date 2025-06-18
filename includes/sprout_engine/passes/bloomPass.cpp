//
// Created by Bellaedris on 18/06/2025.
//

#include "bloomPass.h"

BloomPass::BloomPass(int width, int height, const char *vertShader, const char *fragShader, const char *p_brightVertShader, const char *p_brightFragShader, const char *gaussianFragShader)
        : PostProcessPass(width, height, vertShader, fragShader)
        , brightPassShader(p_brightVertShader, p_brightFragShader)
        , gaussianShader(p_brightVertShader, gaussianFragShader)
        , brightPassHeight(height * BRIGHT_PASS_RESOLUTION_MULTIPLIER)
        , brightPassWidth(width * BRIGHT_PASS_RESOLUTION_MULTIPLIER)
{
    glGenFramebuffers(2, bloomPassFramebuffers);
    for(int i = 0; i < 2; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, bloomPassFramebuffers[i]);

        bloomPassTextures[i] = Texture(brightPassWidth, brightPassHeight, GL_RGBA32F, GL_RGBA, GL_FLOAT);
        glNamedFramebufferTexture(bloomPassFramebuffers[i], GL_COLOR_ATTACHMENT0, bloomPassTextures[i].handle(), 0);
    }
}

void BloomPass::render(Pass *input)
{
    // downsample the framebuffer and only write bright colors
    glViewport(0, 0, brightPassWidth, brightPassHeight);
    brightPassShader.use();
    brightPassShader.uniform_data("threshold", threshold);
    input->activateTexture();
    brightPassShader.uniform_data("framebuffer", 0);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, bloomPassFramebuffers[0]);
    m_renderQuad.draw_strip();

    // apply a gaussian blur horizontally
    gaussianShader.use();
    gaussianShader.uniform_data("horizontal", 0);
    bloomPassTextures[0].use(); // texture 0 holds the bright pass
    brightPassShader.uniform_data("framebuffer", 0);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, bloomPassFramebuffers[1]); // write in the second framebuffer
    m_renderQuad.draw_strip();

    // apply a gaussian blur vertically
    gaussianShader.uniform_data("horizontal", 1);
    bloomPassTextures[1].use(); // texture 0 holds the bright pass

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, bloomPassFramebuffers[0]); // write in the first framebuffer
    m_renderQuad.draw_strip();

    // add the colors back to the original texture
    glViewport(0, 0, m_width, m_height);
    shader.use();
    input->activateTexture(0);
    shader.uniform_data("framebuffer", 0);
    bloomPassTextures[0].use(1);
    shader.uniform_data("blurredBright", 1);
    bindAndDrawEffect();
}

void BloomPass::drawInspector(Camera *camera)
{
    if(ImGui::TreeNode("Bloom"))
    {
        ImGui::Checkbox("Active", &isActive);
        ImGui::InputFloat("Threshold", &threshold, 0.1f, 1.f);

        ImGui::TreePop();
    }
}
