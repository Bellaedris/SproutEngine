//
// Created by Bellaedris on 23/12/2024.
//

#pragma once

#include "sprout_engine/passes/pass.h"

class PostProcessPass : public Pass, public Inspectable
{
protected:
    Shader shader;
    Mesh m_renderQuad;
public:
    bool isActive;

    PostProcessPass(int width, int height, const char* vertShader, const char* fragShader)
        : Pass(width, height)
        , shader(vertShader, fragShader)
        , m_renderQuad(Mesh::generatePlane())
    {
        glGenFramebuffers(1, &m_framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

        m_texture = Texture(width, height, GL_RGBA32F, GL_RGBA, GL_FLOAT);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture.handle(), 0);
    };

    /**
     * \brief Binds either the screen framebuffer or the PP framebuffer, depending on if this post processing effect is
     * the last of the PP chain, then draw the rectangle
     */
    void bindAndDrawEffect()
    {
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

    virtual void render(Pass* input) = 0;
    void drawInspector(Camera *camera) override = 0;
};