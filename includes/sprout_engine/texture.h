#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "stb_image.h"

#include <string>
#include <iostream>

class Texture
{
protected:
	unsigned int id;
public:
	std::string type;
	std::string path;

	Texture() : id(-1) {}
	Texture(const char* path, std::string type = "texture_diffuse", GLenum clamp_method = GL_REPEAT, GLenum filtering_method = GL_LINEAR)
	{
		int width, height, nbChan;
		stbi_set_flip_vertically_on_load(true);
		unsigned char* data = stbi_load(path, &width, &height, &nbChan, 0);

		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, clamp_method);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, clamp_method);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtering_method);

		GLenum format = GL_RGBA;
		switch (nbChan)
		{
		case 3:
			format = GL_RGB;
			break;
		case 4:
			format = GL_RGBA;
			break;
		}

		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
			std::cout << "loaded texture " << path << std::endl;
		}
		else
		{
			std::cout << "error reading the file at: " << path << std::endl;
		}
		stbi_image_free(data);
	};

	inline unsigned int get_id() const { return id; };

	void use(const GLenum texture_unit = GL_TEXTURE0) const
	{
		glActiveTexture(texture_unit);
		glBindTexture(GL_TEXTURE_2D, id);
	};
};