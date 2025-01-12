//
// Created by Bellaedris on 13/06/2024.
//

#include "mesh.h"

Mesh Mesh::generatePlane() {

    std::vector<glm::vec3> vertices =
    {
        {-1, 1, 0},
        {-1, -1, 0},
        {1, 1, 0},
        {1, -1, 0}
    };

    std::vector<glm::vec2> texcoords =
    {
        {0.0f, 1.0f},
        {0.0f, 0.0f},
        {1.0f, 1.0f},
        {1.0f, 0.0f}
    };

    return {vertices, texcoords};
}

Mesh Mesh::generateCube()
{
    std::vector<glm::vec3> cubeVertices = {
            // positions
            { -1.0f, -1.0f, -1.0f },
            { 1.0f,  1.0f, -1.0f },
            { 1.0f, -1.0f, -1.0f },
            { 1.0f,  1.0f, -1.0f },
            { -1.0f, -1.0f, -1.0f },
            { -1.0f,  1.0f, -1.0f },

            {-1.0f, -1.0f,  1.0f},
            {1.0f, -1.0f,  1.0f },
            {1.0f,  1.0f,  1.0f },
            {1.0f,  1.0f,  1.0f },
            {-1.0f,  1.0f,  1.0f},
            {-1.0f, -1.0f,  1.0f},

            {-1.0f,  1.0f,  1.0f},
            {-1.0f,  1.0f, -1.0f},
            {-1.0f, -1.0f, -1.0f},
            {-1.0f, -1.0f, -1.0f},
            {-1.0f, -1.0f,  1.0f},
            {-1.0f,  1.0f,  1.0f},

            {1.0f,  1.0f,  1.0f},
            {1.0f, -1.0f, -1.0f},
            {1.0f,  1.0f, -1.0f},
            {1.0f, -1.0f, -1.0f},
            {1.0f,  1.0f,  1.0f},
            {1.0f, -1.0f,  1.0f},

            {-1.0f, -1.0f, -1.0},
            {1.0f, -1.0f, -1.0f},
            {1.0f, -1.0f,  1.0f},
            {1.0f, -1.0f,  1.0f},
            {-1.0f, -1.0f,  1.0f},
            {-1.0f, -1.0f, -1.0f},

            {-1.0f,  1.0f, -1.0f},
            {1.0f,  1.0f , 1.0f},
            {1.0f,  1.0f, -1.0f},
            {1.0f,  1.0f,  1.0f},
            {-1.0f,  1.0f, -1.0f},
            {-1.0f,  1.0f,  1.0f}
    };

    return {cubeVertices};
}

void Mesh::draw_unindexed()
{
    if (update_data)
        build_buffer();

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, m_positions.size());
    glBindVertexArray(0);
}

void Mesh::draw_strip() {
    if (update_data)
        build_buffer();

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, m_positions.size());
    glBindVertexArray(0);
}

void Mesh::computeBitengants() {
    m_tangents.resize(m_positions.size());
    m_bitangents.resize(m_positions.size());
    for(int i = 0; i < m_indices.size(); i+=3)
    {
        int a = m_indices[i];
        int b = m_indices[i + 1];
        int c = m_indices[i + 2];

        glm::vec3 tan, bitan;
        glm::vec3 e1 = m_positions[b] - m_positions[a];
        glm::vec3 e2 = m_positions[c] - m_positions[a];
        glm::vec2 deltaUV1 = m_texcoords[b] - m_texcoords[a];
        glm::vec2 deltaUV2 = m_texcoords[c] - m_texcoords[a];

        float f = 1.f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
        tan.x = f * (deltaUV2.y * e1.x - deltaUV1.y * e2.x);
        tan.y = f * (deltaUV2.y * e1.y - deltaUV1.y * e2.y);
        tan.z = f * (deltaUV2.y * e1.z - deltaUV1.y * e2.z);

        bitan.x = f * (-deltaUV1.x * e1.x + deltaUV1.x * e2.x);
        bitan.y = f * (-deltaUV1.x * e1.y + deltaUV1.x * e2.y);
        bitan.z = f * (-deltaUV1.x * e1.z + deltaUV1.x * e2.z);

        m_tangents[a] += tan;
        m_tangents[b] += tan;
        m_tangents[c] += tan;

        m_bitangents[a] += bitan;
        m_bitangents[b] += bitan;
        m_bitangents[c] += bitan;
    }

    //renormalize the bitangent and tangent vectors
    for(auto& tan : m_tangents)
        tan = glm::normalize(tan);

    for(auto& bitan : m_bitangents)
        bitan = glm::normalize(bitan);
}

void Mesh::build_buffer() {
    glBindVertexArray(vao);

    // buffer containing all vertices data
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    size_t size = vertex_buffer_size() + normal_buffer_size() + texcoords_buffer_size() + tangent_buffer_size() + bitangent_buffer_size();
    glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);

    //positions subdata
    size_t offset = 0;
    size = vertex_buffer_size();
    glBufferSubData(GL_ARRAY_BUFFER, offset, size, m_positions.data());
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)offset);
    glEnableVertexAttribArray(0);

    //normals
    offset += size;
    size = normal_buffer_size();
    glBufferSubData(GL_ARRAY_BUFFER, offset, size, m_normals.data());
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)offset);
    glEnableVertexAttribArray(1);

    //uvs
    offset += size;
    size = texcoords_buffer_size();
    glBufferSubData(GL_ARRAY_BUFFER, offset, size, m_texcoords.data());
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)offset);
    glEnableVertexAttribArray(2);

    // tangent
    offset += size;
    size = tangent_buffer_size();
    glBufferSubData(GL_ARRAY_BUFFER, offset, size, m_tangents.data());
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void*)offset);
    glEnableVertexAttribArray(3);

    // bitangent
    offset += size;
    size = bitangent_buffer_size();
    glBufferSubData(GL_ARRAY_BUFFER, offset, size, m_bitangents.data());
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, (void*)offset);
    glEnableVertexAttribArray(4);

    //index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_buffer_size(), m_indices.data(), GL_DYNAMIC_DRAW);

    glBindVertexArray(0);

    update_data = false;
}
