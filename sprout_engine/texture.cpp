#include "texture.h"

#include <iostream>

Texture::Texture(const char* path, GLenum clamp_method, GLenum filtering_method)
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

	GLenum format;
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
	}
	else
	{
		std::cout << "error reading the file at: " << path << std::endl;
	}
	stbi_image_free(data);
}

void Texture::use(const GLenum texture_unit) const
{
	glActiveTexture(texture_unit);
	glBindTexture(GL_TEXTURE_2D, id);
}