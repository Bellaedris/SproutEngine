//
// Created by arthu on 26/12/2024.
//

#include "tonemappingPass.h"

TonemappingPass::TonemappingPass(int width, int height)
    : Pass(width, height)
{
    glGenFramebuffers(1, &m_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

    m_texture = Texture(width, height, GL_RGBA32F, GL_RGBA, GL_FLOAT);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture.handle(), 0);

    m_renderQuad = Mesh::generatePlane();
}

void TonemappingPass::render(std::vector<Entity> &entities, const Camera &camera, Shader &shader) {
    if(m_bIsFinal)
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    else
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_framebuffer);

    glDisable(GL_DEPTH_TEST);

    m_renderQuad.draw_strip();

    glEnable(GL_DEPTH_TEST);
}
