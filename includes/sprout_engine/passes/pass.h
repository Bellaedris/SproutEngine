//
// Created by Bellaedris on 23/12/2024.
//

#pragma once

#include "sprout_engine/entity.h"

class Pass
{
protected:
    Texture m_texture;
    GLuint m_framebuffer{};
    int m_width, m_height;

public:
    bool m_bIsFinal{};

    Pass(int width, int height) : m_width(width), m_height(height) {};

    int textureHandle() { return m_texture.handle(); };
    void activateTexture(int textureUnit) const { m_texture.use(textureUnit); };

    void blitToScreen()
    {
        glBlitNamedFramebuffer(m_framebuffer, 0, 0, 0, m_width, m_height, 0, 0, m_width, m_height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
};