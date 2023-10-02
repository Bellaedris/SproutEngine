#pragma once

#include "mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Model
{
protected:
	std::vector<Mesh> meshes;

	void processNode(const aiNode *node, const aiScene *scene);
	Mesh processMesh(const aiMesh *mesh, const aiScene *scene);
public:
	Model() {};
	Model(const char* path);

	void draw(Shader &s) const;
};