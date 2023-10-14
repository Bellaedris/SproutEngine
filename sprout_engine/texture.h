#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "stb_image.h"

#include <string>

class Texture
{
protected:
	unsigned int id;
public:
	std::string type;
	std::string path;

	Texture() : id(-1) {}
	Texture(const char* path, std::string type = "texture_diffuse", GLenum clamp_method = GL_REPEAT, GLenum filtering_method = GL_LINEAR);

	inline unsigned int get_id() const { return id; };
	void use(const GLenum texture_unit = GL_TEXTURE0) const;
};