#include "mesh.h"

Mesh::Mesh(const std::vector<VertexInfo> &vertices, const std::vector<unsigned int> &indices, const std::vector<Texture> &textures, const aiAABB& aabb) : vertices(vertices), indices(indices), textures(textures), aabb(aabb)
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

	// AABB buffers/vao
	glGenVertexArrays(1, &aabb_vao);
	glGenBuffers(1, &aabb_buff);

	glBindVertexArray(aabb_vao);

	// buffer containing all vertices data
	float bounds[72] =
	{
		//front face
		aabb.mMin.x, aabb.mMin.y, aabb.mMin.z, 
		aabb.mMax.x, aabb.mMin.y, aabb.mMin.z,
		aabb.mMax.x, aabb.mMin.y, aabb.mMin.z,
		aabb.mMax.x, aabb.mMax.y, aabb.mMin.z,
		aabb.mMax.x, aabb.mMax.y, aabb.mMin.z,
		aabb.mMin.x, aabb.mMax.y, aabb.mMin.z,
		aabb.mMin.x, aabb.mMax.y, aabb.mMin.z,
		aabb.mMin.x, aabb.mMin.y, aabb.mMin.z,

		//back face
		aabb.mMin.x, aabb.mMin.y, aabb.mMax.z,
		aabb.mMax.x, aabb.mMin.y, aabb.mMax.z,
		aabb.mMax.x, aabb.mMin.y, aabb.mMax.z,
		aabb.mMax.x, aabb.mMax.y, aabb.mMax.z,
		aabb.mMax.x, aabb.mMax.y, aabb.mMax.z,
		aabb.mMin.x, aabb.mMax.y, aabb.mMax.z,
		aabb.mMin.x, aabb.mMax.y, aabb.mMax.z,
		aabb.mMin.x, aabb.mMin.y, aabb.mMax.z,

		// edges
		aabb.mMin.x, aabb.mMin.y, aabb.mMin.z,
		aabb.mMin.x, aabb.mMin.y, aabb.mMax.z,
		aabb.mMax.x, aabb.mMin.y, aabb.mMin.z,
		aabb.mMax.x, aabb.mMin.y, aabb.mMax.z,
		aabb.mMin.x, aabb.mMax.y, aabb.mMin.z,
		aabb.mMin.x, aabb.mMax.y, aabb.mMax.z,
		aabb.mMax.x, aabb.mMax.y, aabb.mMin.z,
		aabb.mMax.x, aabb.mMax.y, aabb.mMax.z,
	};

	glBindBuffer(GL_ARRAY_BUFFER, aabb_buff);
	glBufferData(GL_ARRAY_BUFFER, sizeof(bounds), bounds, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
}

void Mesh::draw(Shader& s/*, const Frustum& frustum*/) const
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
}

void Mesh::drawAABB(Shader& s) const
{
	s.use();

	// draw AABB
	glBindVertexArray(aabb_vao);
	glDrawArrays(GL_LINES, 0, 72);
}