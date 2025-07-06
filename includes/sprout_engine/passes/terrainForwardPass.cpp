//
// Created by Bellaedris on 01/07/2025.
//

#include "terrainForwardPass.h"

TerrainForwardPass::TerrainForwardPass(int width, int height)
        : DrawPass(width, height)
{
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    glGenFramebuffers(1, &m_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

    m_texture = Texture(width, height, GL_RGBA32F, GL_RGBA, GL_FLOAT);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture.handle(), 0);

    m_depthTexture = Texture::buildDepthTexture(width, height);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTexture.handle(), 0);
}

void TerrainForwardPass::render(Mesh &terrain)
{
    if(m_bIsFinal)
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    else
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_framebuffer);
    glViewport(0, 0, m_width, m_height);
    glClearColor(.1f, .1f, .1f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    terrain.drawNonPBR();
}

void TerrainForwardPass::useDepthTexture(int unit)
{
    m_depthTexture.use(unit);
}
