#include "model.h"

#include <iostream>

Model::Model(const char* path)
{
	Assimp::Importer importer;
	auto scene = importer.ReadFile(path, aiProcessPreset_TargetRealtime_Fast | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "error assimp: " << importer.GetErrorString() << std::endl;
		return;
	}

	processNode(scene->mRootNode, scene);
}

void Model::processNode(const aiNode* node, const aiScene* scene)
{
	for (int i = 0; i < node->mNumMeshes; i++)
	{
		meshes.push_back(
			processMesh(scene->mMeshes[node->mMeshes[i]], scene)
		);
	}
	for (int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(const aiMesh* mesh, const aiScene* scene)
{
	std::vector<VertexInfo> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	// process all vertex data
	for (int i = 0; i < mesh->mNumVertices; i++)
	{
		VertexInfo vertex;
		vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);

		if (mesh->mTextureCoords[0])
			vertex.uv = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		else
			vertex.uv = glm::vec2(0.f, 0.f);

		vertices.push_back(vertex);
	}

	//process indices
	for (int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	//TODO process textures


	return Mesh(vertices, indices/*, textures*/);
}

void Model::draw(Shader& s) const
{
	for (Mesh m : meshes)
	{
		m.draw(s);
	}
}