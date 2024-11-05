//
// Created by arpradier on 23/09/2024.
//


#pragma once

#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// class Buffer {
//     GLuint id;
//
//
// };

template<typename T>
class UniformBuffer //: public Buffer
{
private:
    unsigned int id{};
    unsigned int binding{};

public:
    UniformBuffer()
    {
        glGenBuffers(1, &id);
    }

    ~UniformBuffer()
    {
        glDeleteBuffers(1, &id);
    }

    void Allocate(size_t size)
    {
        glNamedBufferData(id, sizeof(T) * size, nullptr, GL_STATIC_DRAW);
    }

    void Update(const std::vector<T>& data)
    {
        glNamedBufferSubData(id, 0, sizeof(T) * data.size(), data.data());
    }

    void Bind(GLuint bindingPoint) const
    {
        glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, id);
    }
};

