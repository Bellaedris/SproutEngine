//
// Created by Bellaedris on 08/04/2024.
//

#ifndef SPROUTENGINE_SKYBOX_H
#define SPROUTENGINE_SKYBOX_H

#endif //SPROUTENGINE_SKYBOX_H

#include "cubemap.h"
#include "mesh.h"
#include "shader.h"

class Skybox
{
public:
    Skybox() : m_vao(-1), m_vertexBuffer(-1) {};

    Skybox(const std::string path) : m_cubemap(Cubemap(path.c_str()))
    {
        initBuffers();
    };

    Skybox(const std::array<std::string, 6>& paths) : m_cubemap(Cubemap(paths))
    {
        initBuffers();
    };

    void initBuffers()
    {
        float cubeVertices[] = {
                // positions
                -1.0f,  1.0f, -1.0f,
                -1.0f, -1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,
                1.0f,  1.0f, -1.0f,
                -1.0f,  1.0f, -1.0f,

                -1.0f, -1.0f,  1.0f,
                -1.0f, -1.0f, -1.0f,
                -1.0f,  1.0f, -1.0f,
                -1.0f,  1.0f, -1.0f,
                -1.0f,  1.0f,  1.0f,
                -1.0f, -1.0f,  1.0f,

                1.0f, -1.0f, -1.0f,
                1.0f, -1.0f,  1.0f,
                1.0f,  1.0f,  1.0f,
                1.0f,  1.0f,  1.0f,
                1.0f,  1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,

                -1.0f, -1.0f,  1.0f,
                -1.0f,  1.0f,  1.0f,
                1.0f,  1.0f,  1.0f,
                1.0f,  1.0f,  1.0f,
                1.0f, -1.0f,  1.0f,
                -1.0f, -1.0f,  1.0f,

                -1.0f,  1.0f, -1.0f,
                1.0f,  1.0f, -1.0f,
                1.0f,  1.0f,  1.0f,
                1.0f,  1.0f,  1.0f,
                -1.0f,  1.0f,  1.0f,
                -1.0f,  1.0f, -1.0f,

                -1.0f, -1.0f, -1.0f,
                -1.0f, -1.0f,  1.0f,
                1.0f, -1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,
                -1.0f, -1.0f,  1.0f,
                1.0f, -1.0f,  1.0f
        };

        glGenVertexArrays(1, &m_vao);
        glGenBuffers(1, &m_vertexBuffer);
        glBindVertexArray(m_vao);

        glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);
    }

    void draw(Shader& p_shader)
    {
        p_shader.use();
        glBindVertexArray(m_vao);
        m_cubemap.bind();
        glDrawArrays(GL_TRIANGLES, 0, 36);
    };

protected:
    Cubemap m_cubemap;
    GLuint m_vao;
    GLuint  m_vertexBuffer;
};