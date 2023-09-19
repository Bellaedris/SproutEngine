#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"

class Texture
{
protected:
	unsigned int id;
public:
	Texture() : id(-1) {}
	Texture(const char* path, GLenum clamp_method = GL_REPEAT, GLenum filtering_method = GL_LINEAR);

	void use(const GLenum texture_unit = GL_TEXTURE0) const;
};