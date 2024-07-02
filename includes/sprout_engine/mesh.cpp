//
// Created by Bellaedris on 13/06/2024.
//

#include "mesh.h"

Mesh Mesh::generatePlane() {

    std::vector<glm::vec3> vertices =
            {
                    {-1, -1, 0},
                    {-1, 1, 0},
                    {1, -1, 0},
                    {1, 1, 0}
            };

    std::vector<glm::vec2> texcoords =
            {
                    {0.0f, 0.0f},
                    {0.0f, 1.0f},
                    {1.0f, 0.0f},
                    {1.0f, 1.0f},
            };

    return {vertices, texcoords};
}

void Mesh::draw_flat(Shader &s)
{
    if (update_data)
        build_buffer();

    s.use();

    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Mesh::draw_strip(Shader &s) {
    if (update_data)
        build_buffer();

    s.use();

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, m_positions.size());
    glBindVertexArray(0);
}
