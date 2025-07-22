//
// Created by Bellaedris on 01/07/2025.
//

#include "terrainForwardPass.h"

TerrainForwardPass::TerrainForwardPass(int width, int height, const char* vertShader, const char* fragShader)
        : DrawPass(width, height)
        , skyboxShader(vertShader, fragShader)
        , skyboxCube(Mesh::generateCube())
{
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    glGenFramebuffers(1, &m_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

    m_texture = Texture(width, height, GL_RGBA32F, GL_RGBA, GL_FLOAT);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture.handle(), 0);

    m_depthTexture = Texture::buildDepthTexture(width, height);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTexture.handle(), 0);
}

void TerrainForwardPass::render(Mesh &terrain, Camera* camera)
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_framebuffer);
    glViewport(0, 0, m_width, m_height);
    glClearColor(.1f, .1f, .1f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    terrain.drawNonPBR();

    // then draw the skybox
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);
    skyboxShader.use();
    skyboxShader.uniform_data("viewMatrix", glm::mat4(glm::mat3(camera->view())));
    skyboxShader.uniform_data("projectionMatrix", camera->projection());
    skyboxShader.uniform_data("sunDir", sunDir);
    skyboxShader.uniform_data("sunColor", sunColor);
    skyboxShader.uniform_data("sunSize", sunSize);
    skyboxCube.draw_unindexed();
    glDepthMask(GL_TRUE);
}

void TerrainForwardPass::useDepthTexture(int unit)
{
    m_depthTexture.use(unit);
}

void TerrainForwardPass::BuildTextures()
{
    m_texture = Texture(m_width, m_height, GL_RGBA32F, GL_RGBA, GL_FLOAT);
    glNamedFramebufferTexture(m_framebuffer, GL_COLOR_ATTACHMENT0, m_texture.handle(), 0);

    m_depthTexture = Texture::buildDepthTexture(m_width, m_height);
    glNamedFramebufferTexture(m_framebuffer, GL_DEPTH_ATTACHMENT, m_depthTexture.handle(), 0);
}