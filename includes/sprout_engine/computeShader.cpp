//
// Created by arpradier on 12/09/2024.
//

#include "computeShader.h"

ComputeShader::ComputeShader(const char* p_filename)
{
    const char* shaderSource = read_file(p_filename)->c_str();

    if (shaderSource != nullptr)
    {
        unsigned int compute = glCreateShader(GL_COMPUTE_SHADER);
        glShaderSource(compute, 1, &shaderSource, NULL);
        glCompileShader(compute);

        int success;
        char infoLog[512];
        glGetShaderiv(compute, GL_COMPILE_STATUS, &success);

        if (!success)
        {
            glGetShaderInfoLog(compute, 512, NULL, infoLog);
            std::cout << "error compiling compute shader at " << p_filename << ": " << infoLog << std::endl;
        }

        id = glCreateProgram();
        glAttachShader(id, compute);
        glLinkProgram(id);

        glGetShaderiv(id, GL_LINK_STATUS, &success);

        if (!success)
        {
            glGetShaderInfoLog(id, 512, NULL, infoLog);
            std::cout << "error linking compute shader at " << p_filename << ": " << infoLog << std::endl;
        }
    }
}

void ComputeShader::use()
{
    glUseProgram(id);
}

void ComputeShader::dispatch(unsigned int x, unsigned int y, unsigned int z)
{
    glDispatchCompute(x, y, z);
}
