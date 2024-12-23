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
    Skybox(const std::string path);
    Skybox(const std::array<std::string, 6>& paths);

    void initBuffers();

    void draw(Shader& p_shader);;

protected:
    Cubemap m_cubemap;
    GLuint m_vao;
    GLuint  m_vertexBuffer;
};