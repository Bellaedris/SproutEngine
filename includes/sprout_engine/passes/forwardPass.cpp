//
// Created by Bellaedris on 26/12/2024.
//

#include <stack>
#include <queue>
#include "forwardPass.h"

ForwardPass::ForwardPass(int width, int height)
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

void ForwardPass::render(
        const std::unique_ptr<Entity> &rootNode,
        const Camera &camera,
        const glm::mat4 &lightspaceMatrix,
        Shader &shader
)
{
    if(m_bIsFinal)
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    else
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_framebuffer);
    glViewport(0, 0, m_width, m_height);
    glClearColor(.1f, .1f, .1f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 view = camera.view();
    glm::mat4 projection = camera.projection();
    glm::mat4 inverseViewMatrix = glm::inverse(view);

    shader.uniform_data("viewMatrix", view);
    shader.uniform_data("projectionMatrix", projection);
    shader.uniform_data("inverseViewMatrix", inverseViewMatrix);
    shader.uniform_data("lightspaceMatrix", lightspaceMatrix);

    // draw all nodes recursively
    std::queue<Entity*> queue;
    queue.push(rootNode.get());
    while(!queue.empty())
    {
        Entity* current = queue.front();
        std::vector<std::shared_ptr<Entity>>& currentChildren = current->GetChildren();
        for (auto& entity: currentChildren)
        {
            queue.push(entity.get());
            glm::mat4 model = entity->getTransform().getModelMatrix();

            glm::mat4 normalMatrix = glm::transpose(glm::inverse(model));
            shader.uniform_data("modelMatrix", model);
            shader.uniform_data("normalMatrix", normalMatrix);

            entity->draw(shader, camera.getFrustum(), entity->getTransform());
        }
        queue.pop();
    }
}

