#pragma once

#include <vector>
#include <string>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Texture.hpp"
#include "Model.hpp"

namespace VoxelEngine
{

class AssetManager
{
public:
	AssetManager();
	~AssetManager();
	Texture& LoadTexture(std::string path, std::string type);
	uint32_t LoadCubemap(std::vector<std::string> faces);

	Model* LoadModel(std::string path);

private:
	static std::vector<Texture> m_LoadedTextures;

	uint32_t LoadTextureFromFile(const std::string& fullpath, int32_t type, bool flip);
	uint32_t LoadTextureFromFile(const std::string& path, int32_t type, bool flip, const std::string& directory);

	void ProcessNode(aiNode* node, const aiScene* scene, Model& model, std::string directory);
	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene, std::string directory);
	std::vector<Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName, std::string directory);
};
};
