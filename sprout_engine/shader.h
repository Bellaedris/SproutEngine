#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

#include "glm.hpp"
#include "../glm/gtc/type_ptr.hpp"

class Shader
{
protected:
	unsigned int shader;

public:
	Shader();
	Shader(const char* vert_path, const char* frag_path);

	void use();

	// uniform data
	void uniform_data(const char* name, const glm::mat4) const; // mat4
	void uniform_data(const char* name, const float x, const float y, const float z, const float w) const; // vec4
	void uniform_data(const char* name, const float x, const float y, const float z) const; // vec3
	void uniform_data(const char* name, const float x) const; // float
	void uniform_data(const char* name, const int x) const; // int
};