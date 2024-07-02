#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <sstream>

inline std::string* read_file(const char* filename)
{
	std::ifstream file(filename, std::ifstream::in);
	if (file)
	{
		//read the shader file
		std::stringbuf s;
		file.get(s, 0);
		file.close();

		return new std::string(s.str());
	}
}

class Shader
{
protected:
	unsigned int shader;

public:
	Shader() : shader(-1) {};

	Shader(const char* vert_path, const char* frag_path)
	{
		const char* vert_data = read_file(vert_path)->c_str();
		const char* frag_data = read_file(frag_path)->c_str();

		if (vert_data == nullptr)
		{
			fprintf(stderr, "Error when trying to read file at %s\n", vert_path);
			return;
		}

		if (frag_data == nullptr)
		{
			fprintf(stderr, "Error when trying to read file at %s\n", frag_path);
			return;
		}

		unsigned int vert = glCreateShader(GL_VERTEX_SHADER);

		//compile the vertex shader
		glShaderSource(vert, 1, &vert_data, NULL);
		glCompileShader(vert);

		int success;
		char infoLog[512];
		glGetShaderiv(vert, GL_COMPILE_STATUS, &success);

		if (!success)
		{
			glGetShaderInfoLog(vert, 512, NULL, infoLog);
			std::cout << "error compiling shaders at " << vert_path << ": " << infoLog << std::endl;
		}

		//compile the fragment shader
		unsigned int frag = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(frag, 1, &frag_data, NULL);
		glCompileShader(frag);

		glGetShaderiv(frag, GL_COMPILE_STATUS, &success);

		if (!success)
		{
			glGetShaderInfoLog(frag, 512, NULL, infoLog);
			std::cout << "error compiling shaders at " << frag_path << ": " << infoLog << std::endl;
		}

		//create the actual shader
		shader = glCreateProgram();
		glAttachShader(shader, vert);
		glAttachShader(shader, frag);
		glLinkProgram(shader);

		glGetShaderiv(shader, GL_LINK_STATUS, &success);

		if (!success)
		{
			glGetShaderInfoLog(vert, 512, NULL, infoLog);
			std::cout << "error linking shaders at " << vert_path << " and " << vert_path << ": " << infoLog << std::endl;
		}

		glDeleteShader(vert);
		glDeleteShader(frag);
	};

	void use()
	{
		glUseProgram(shader);
	};

	// uniform data
#pragma region uniform data
	GLint get_location(const std::string &name) const
	{
		return glGetUniformLocation(shader, name.c_str());
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
#pragma endregion
};