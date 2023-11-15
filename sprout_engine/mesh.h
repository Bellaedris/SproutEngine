#pragma once

#include "texture.h"
#include "shader.h"
#include "camera.h"
#include "bounds.h"

#include <glm.hpp>
#include <vector>

#include <assimp/aabb.h>

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
	std::vector<Texture> textures;

	AABB aabb;
	unsigned int aabb_buff;
	unsigned int aabb_vao;
	Shader aabb_shader;
	
	unsigned int buffer;
	unsigned int index_buffer;
	unsigned int vao;

public:
	Mesh(const std::vector<VertexInfo> &vertices, const std::vector<unsigned int> &indices, const std::vector<Texture> &textures, const aiAABB &aabb);

	void draw(Shader &s, const Frustum &frustum, const Transform& transform) const;
	void drawAABB(Shader& s, const glm::mat3& transform, const glm::vec3& translation);
	void draw_debug_frustum(Shader& s, const Frustum& frustum, const Transform& transform);
};