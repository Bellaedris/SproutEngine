#include "shader.h"
#include "utils.h"

#include <iostream>
#include <fstream>
#include <sstream>

std::string* read_file(const char* filename)
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

Shader::Shader() : shader(-1) {}

Shader::Shader(const char* vert_path, const char* frag_path)
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

	glLinkProgram(shader);
}

void Shader::use()
{
	glUseProgram(shader);
}

#pragma region uniform data
void Shader::uniform_data(const char* name, const glm::mat4 mat) const
{
	glUniformMatrix4fv(glGetUniformLocation(shader, name), 1, false, glm::value_ptr(mat));
}

void Shader::uniform_data(const char* name, const float x, const float y, const float z, const float w) const
{
	glUniform4f(glGetUniformLocation(shader, name), x, y, z, w);
}

void Shader::uniform_data(const char* name, const float x, const float y, const float z) const
{
	glUniform3f(glGetUniformLocation(shader, name), x, y, z);
}

void Shader::uniform_data(const char* name, const float x) const
{
	glUniform1f(glGetUniformLocation(shader, name), x);
}

void Shader::uniform_data(const char* name, const int x) const
{
	glUniform1i(glGetUniformLocation(shader, name), x);
}
#pragma endregion