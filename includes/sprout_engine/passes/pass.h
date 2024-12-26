//
// Created by Bellaedris on 23/12/2024.
//

#pragma once

#include "sprout_engine/entity.h"

class Pass
{
protected:
    Texture m_texture;
    GLuint m_framebuffer;
    int m_width, m_height;

public:
    bool m_bIsFinal{};

    Pass(int width, int height) : m_width(width), m_height(height) {};
    virtual void render(std::vector<Entity> &entities, const Camera &camera, Shader &shader) = 0;

    int textureHandle() { return m_texture.get_id(); };
    void activateTexture(GLenum textureUnit = GL_TEXTURE0) { m_texture.use(); };

    void blitToScreen()
    {
        glBlitNamedFramebuffer(m_framebuffer, 0, 0, 0, m_width, m_height, 0, 0, m_width, m_height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
};