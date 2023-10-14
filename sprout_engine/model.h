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
	std::vector<Texture> loadMaterialTexture(const aiMaterial* mat, aiTextureType type, std::string typename);
public:
	Model() {};
	Model(const std::string path, bool flip_uv = false);

	void draw(Shader &s) const;
	void drawAABB(Shader& s) const;
};