//
// Created by Bellaedris on 30/03/2025.
//

#include <queue>
#include "shadowmappingPass.h"

ShadowmappingPass::ShadowmappingPass(int width, int height)
        : DrawPass(width, height)
{
    glGenFramebuffers(1, &m_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

    m_texture = Texture::buildDepthTexture(width, height);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_texture.handle(), 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
}

void ShadowmappingPass::render(const std::unique_ptr<Entity> &rootNode, const DirectionalLight &mainLight, Shader &shader)
{
    //draw the scene from the light pov
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_framebuffer);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glViewport(0, 0, m_width, m_height);
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 light_view = glm::lookAt({0, 10, 0}, mainLight.getDirection(), {0, 1, 0});
    glm::mat4 light_projection = glm::ortho(-m_orthographicZoom, m_orthographicZoom, -m_orthographicZoom, m_orthographicZoom, .1f, 1000.f);

    shader.use();
    shader.uniform_data("viewMatrix", light_view);
    shader.uniform_data("projectionMatrix", light_projection);

    std::queue<Entity*> queue;
    queue.push(rootNode.get());
    while(!queue.empty())
    {
        Entity* current = queue.front();
        std::vector<std::shared_ptr<Entity>>& currentChildren = current->GetChildren();
        for (auto& entity: currentChildren)
        {
            shader.uniform_data("modelMatrix", entity->getTransform().getModelMatrix());

            entity->draw(shader);
        }
        queue.pop();
    }
}

void ShadowmappingPass::setResolution(int size)
{
    m_width = size;
    m_height = size;

    // update framebuffer data
    m_texture = Texture::buildDepthTexture(m_width, m_height);
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_texture.handle(), 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowmappingPass::setOrthographicZoom(float zoom)
{
    m_orthographicZoom = zoom;
}

void ShadowmappingPass::BuildTextures()
{
    m_texture = Texture::buildDepthTexture(m_width, m_height);
    glNamedFramebufferTexture(m_framebuffer, GL_DEPTH_ATTACHMENT, m_texture.handle(), 0);
}
