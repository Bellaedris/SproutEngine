#pragma once

#include "texture.h"
#include "shader.h"
#include "camera.h"
#include "bounds.h"

#include <glm/glm.hpp>
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
	Mesh(const std::vector<VertexInfo> &vertices, const std::vector<unsigned int> &indices, const std::vector<Texture> &textures, const aiAABB &in_aabb)
		: vertices(vertices), indices(indices), textures(textures)
	{
		// Mesh buffers/vao
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &buffer);
		glGenBuffers(1, &index_buffer);

		glBindVertexArray(vao);

		// buffer containing all vertices data
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexInfo), vertices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexInfo), 0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexInfo), (void*)offsetof(VertexInfo, normal));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexInfo), (void*)offsetof(VertexInfo, uv));
		glEnableVertexAttribArray(2);

		glBindVertexArray(0);

		//init AABB
		aabb = AABB(glm::vec3(in_aabb.mMin.x, in_aabb.mMin.y, in_aabb.mMin.z), glm::vec3(in_aabb.mMax.x, in_aabb.mMax.y, in_aabb.mMax.z));

		// AABB buffers/vao
		glGenVertexArrays(1, &aabb_vao);
		glGenBuffers(1, &aabb_buff);

		glBindVertexArray(aabb_vao);

		// buffer containing all vertices data
		auto aabb_bounds = aabb.getVertices();
		glm::vec3 bounds[24] =
		{
			//front face
			aabb_bounds[0],
			aabb_bounds[1],
			aabb_bounds[1],
			aabb_bounds[2],
			aabb_bounds[2],
			aabb_bounds[3],
			aabb_bounds[3],
			aabb_bounds[0],

			//back face
			aabb_bounds[4],
			aabb_bounds[5],
			aabb_bounds[5],
			aabb_bounds[6],
			aabb_bounds[6],
			aabb_bounds[7],
			aabb_bounds[7],
			aabb_bounds[4],

			// edges
			aabb_bounds[0],
			aabb_bounds[4],
			aabb_bounds[1],
			aabb_bounds[5],
			aabb_bounds[2],
			aabb_bounds[6],
			aabb_bounds[3],
			aabb_bounds[7],
		};

		glBindBuffer(GL_ARRAY_BUFFER, aabb_buff);
		glBufferData(GL_ARRAY_BUFFER, sizeof(bounds), bounds, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		glBindVertexArray(0);
	};

	void draw(Shader& s) const
	{
		s.use();

		// bind textures
		unsigned int diffuse_id = 1;
		unsigned int specular_id = 1;
		for (int i = 0; i < textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i); //activate the correct texture unit
			std::string number;
			std::string name = textures[i].type;
			if (name == "texture_diffuse")
				number = std::to_string(diffuse_id++);
			if (name == "texture_specular")
				number = std::to_string(specular_id++);
			s.uniform_data((name + number).c_str(), i); // give the correct location to each texture
			glBindTexture(GL_TEXTURE_2D, textures[i].get_id());
		}
		glActiveTexture(GL_TEXTURE0);

		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	};

	void draw(Shader &s, const Frustum &frustum, const Transform& transform) const
	{
		//check if the aabb is visible (inside the frustum)
		//convert aabb to center-extents
		if (!aabb.isOnFrustum(frustum, transform))
			return;
		else
			draw(s);
	};

	void drawAABB(Shader& s, const glm::mat3& transform, const glm::vec3& translation)
	{
		s.use();

		// draw AABB
		glBindVertexArray(aabb_vao);
		glDrawArrays(GL_LINES, 0, 24);
		glBindVertexArray(0);
	};

	void draw_debug_frustum(Shader& s, const Frustum& frustum, const Transform& transform)
	{
		s.use();

		// test collision and give the correct color
		Plane p(glm::vec3(0, 0, 0), glm::vec3(1, 0, 0));
		if (aabb.isOnFrustum(frustum, transform))
		{
			s.uniform_data("color", 0.f, 1.f, 0.f);
		}
		else
		{
			s.uniform_data("color", 1.f, 0.f, 0.f);
		}

		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	};
};