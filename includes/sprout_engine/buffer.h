//
// Created by arpradier on 23/09/2024.
//


#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

// class Buffer {
//     GLuint id;
//
//
// };

class UniformBuffer //: public Buffer
{
private:
    unsigned int id{};

public:
    UniformBuffer() = default;
    UniformBuffer(size_t size);

    void Allocate(const void* data, size_t size);
};


