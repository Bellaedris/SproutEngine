#pragma once

#include "texture.h"
#include "shader.h"

#include <glm.hpp>
#include <vector>

struct VertexInfo
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 uv;
};

class Mesh
{
protected:
	std::vector<VertexInfo> vertices;
	std::vector<unsigned int> indices;
	// std::vector<Texture> textures;
	
	unsigned int buffer;
	unsigned int index_buffer;
	unsigned int vao;

public:
	Mesh(const std::vector<VertexInfo> vertices, const std::vector<unsigned int> indices/*, const std::vector<Texture> textures*/);

	void draw(Shader &s) const;
};