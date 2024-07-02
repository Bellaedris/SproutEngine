//
// Created by Bellaedris on 16/06/2024.
//

#include "line.h"

void LineRenderer::addLine(const glm::vec3 &from, const glm::vec3 &to)
{
    lines.push_back({from, to});
    updated = true;
}

LineRenderer::LineRenderer(const ICamera* camera)
    : camera(camera)
{
    s = std::make_unique<Shader>("debug_lines.vs", "debug_lines.fs");

    glCreateVertexArrays(1, &vao);
    glGenBuffers(1, &buffer);
}

LineRenderer::~LineRenderer()
{
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &buffer);
}

void LineRenderer::draw()
{
    if(updated)
        createBuffers();
    glBindVertexArray(vao);

    s->use();
    glm::mat4 mvpMatrix = camera->projection() * camera->view();
    s->uniform_data("mvpMatrix", mvpMatrix);

    glDrawArrays(GL_LINES, 0, lines.size() * 2.f);
    glBindVertexArray(0);
}

void LineRenderer::clear()
{
    lines.clear();
}

void LineRenderer::createBuffers()
{
    updated = false;

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Line) * lines.size(), lines.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
