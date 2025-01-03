#pragma once

#include "mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Model
{
protected:
	std::vector<Mesh> meshes;
	std::vector<Texture> loaded_textures;
	std::string directory;

	void processNode(const aiNode *node, const aiScene *scene);

	Mesh processMesh(const aiMesh *mesh, const aiScene *scene);

	std::vector<Texture> loadMaterialTexture(const aiMaterial* mat, aiTextureType type, std::string type_name);

public:
	Model() = default;
	explicit Model(const std::string& path, bool flip_uv = false);

    const std::vector<Mesh>& getMeshes() const {return meshes;};

	void draw(Shader &s, const Frustum &f, const Transform &transform);

	void draw(Shader& s);

	void drawAABB(Shader& s, const glm::mat3& transform, const glm::vec3& translation);

	void draw_debug_frustum(Shader& s, const Frustum& frustum, const Transform& transform);
	;
};