//
// Created by Bellaedris on 23/12/2024.
//

#include "depthPrePass.h"

DepthPrePass::DepthPrePass(int width, int height)
    : DrawPass(width, height)
{
    m_texture = Texture::buildDepthTexture(width, height);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_texture.handle(), 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
}

void DepthPrePass::render(std::vector<Entity> &entities, const Camera &camera, Shader &shader)
{
    //draw the scene from the light pov
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_framebuffer);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glViewport(0, 0, m_width, m_height);
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 light_view = camera.view();
    glm::mat4 light_projection = glm::ortho(-m_orthographicZoom, m_orthographicZoom, -m_orthographicZoom, m_orthographicZoom, .1f, 100000.f); // glm::perspective(glm::radians(fov), w / h, .1f, 100.f);

    for(auto& entity : entities)
    {
        shader.use();
        shader.uniform_data("mvpMatrix", light_projection * light_view * entity.getTransform().getModelMatrix());

        entity.draw(shader);
    }
}
