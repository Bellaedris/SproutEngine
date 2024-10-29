//
// Created by arpradier on 12/09/2024.
//

#pragma once

#include <glad/glad.h>

#include "shader.h"

class ComputeShader
{
protected:
    unsigned int id;

public:
    ComputeShader() : id(-1) {};
    explicit ComputeShader(const char* p_filename);

    void use();
    void dispatch(unsigned int x, unsigned int y, unsigned int z);

    [[nodiscard]] unsigned int GetId() const {return id;}

    GLint get_location(const std::string &name) const
    {
        return glGetUniformLocation(id, name.c_str());
    }

    void uniform_data(const std::string& name, const glm::mat4 mat) const
    {
        glUniformMatrix4fv(get_location(name), 1, false, glm::value_ptr(mat));
    }

    void uniform_data(const std::string& name, const float x, const float y, const float z, const float w) const
    {
        glUniform4f(get_location(name), x, y, z, w);
    }

    void uniform_data(const std::string& name, const glm::vec4& data) const
    {
        glUniform4f(get_location(name), data.x, data.y, data.z, data.w);
    }

    void uniform_data(const std::string& name, const glm::vec3& data) const
    {
        glUniform3f(get_location(name), data.x, data.y, data.z);
    }

    void uniform_data(const std::string &name, const float x, const float y, const float z) const
    {
        glUniform3f(get_location(name), x, y, z);
    }

    void uniform_data(const std::string& name, const float x) const
    {
        glUniform1f(get_location(name), x);
    }

    void uniform_data(const std::string& name, const int x) const
    {
        glUniform1i(get_location(name), x);
    }
};