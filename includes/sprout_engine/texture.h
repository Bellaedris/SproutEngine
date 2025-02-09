#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <sprout_engine/image.h>

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

	Texture()
    {
        glGenTextures(1, &id);
    }

	Texture(const char* path, std::string type = "texture_diffuse", GLenum clamp_method = GL_REPEAT, GLenum filtering_method = GL_LINEAR) : type(type), path(path)
	{
		int width, height, nbChan;
		stbi_set_flip_vertically_on_load(true);
		unsigned char* data = stbi_load(path, &width, &height, &nbChan, 0);

		if (data)
		{
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

            GLenum internalFormat = GL_RGB;
            if(type == "texture_diffuse")
                internalFormat = GL_SRGB;

			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
			std::cout << "loaded texture " << path << std::endl;
		}
		else
		{
			std::cout << "error reading the file at: " << path << std::endl;
		}
		stbi_image_free(data);
	};

    Texture(unsigned char* data, int width, int height, int nbChan, std::string type = "texture_diffuse", GLenum clamp_method = GL_REPEAT, GLenum filtering_method = GL_LINEAR) : type(type), path(path)
    {
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

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Set alignment to 1 byte
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    };

    Texture(float* data, int width, int height, int nbChan, std::string type = "texture_diffuse", GLenum clamp_method = GL_REPEAT, GLenum filtering_method = GL_LINEAR) : type(type), path(path)
    {
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

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Set alignment to 1 byte
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    };

	Texture(const Image &data, std::string type = "texture_diffuse", GLenum clamp_method = GL_REPEAT, GLenum filtering_method = GL_LINEAR) : type(type), path(path)
	{
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, clamp_method);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, clamp_method);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtering_method);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, data.width(), data.height(), 0, GL_RGBA, GL_FLOAT, data.data());
		glGenerateMipmap(GL_TEXTURE_2D);
	};

	Texture(const int width, const int height, GLenum internal_format, GLenum format = GL_RGBA, GLenum format_type = GL_UNSIGNED_BYTE, std::string type = "texture_diffuse", GLenum clamp_method = GL_REPEAT, GLenum filtering_method = GL_LINEAR) : type(type), path("empty")
	{
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);

		glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, format_type, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, clamp_method);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, clamp_method);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, filtering_method);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, filtering_method);
        if (clamp_method == GL_CLAMP_TO_BORDER)
        {
            float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
        }

		glGenerateMipmap(GL_TEXTURE_2D);
	};

	template<typename T>
	Texture(int width, int height, const std::vector<T>& data)
	{
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);

		// Allocate space for the texture (RGB format)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, data.data());

		// Set texture parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}


	Texture(int width, int height)
	{
		glGenTextures(1, &id);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, id);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA,
		             GL_FLOAT, NULL);
	}

    static Texture buildDepthTexture(int width, int height)
    {
        Texture t;
        glBindTexture(GL_TEXTURE_2D, t.handle());

        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        return t;
    }

    static Texture loadHDRTexture(const char* path)
    {
        Texture t;
        int width, height, nbChan;
        stbi_set_flip_vertically_on_load(true);
        float* data = stbi_loadf(path, &width, &height, &nbChan, 0);

        if (data)
        {
            glBindTexture(GL_TEXTURE_2D, t.handle());

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);
            std::cout << "loaded hdr texture " << path << std::endl;

            glBindTexture(GL_TEXTURE_2D, 0);
        }
        else
        {
            std::cout << "error reading the HDR image file at: " << path << std::endl;
        }
        stbi_image_free(data);

        return t;
    }

	inline unsigned int handle() const { return id; };

	void use(const GLenum texture_unit = GL_TEXTURE0) const
	{
		glActiveTexture(texture_unit);
		glBindTexture(GL_TEXTURE_2D, id);
	};


};