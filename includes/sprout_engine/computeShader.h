//
// Created by arpradier on 12/09/2024.
//

#pragma once

#include <glad/glad.h>

#include "shader.h"

class ComputeShader {
protected:
    unsigned int id;

public:
    ComputeShader() : id(-1) {};
    explicit ComputeShader(const char* p_filename);

    void use();
    void dispatch(unsigned int x, unsigned int y, unsigned int z);
};