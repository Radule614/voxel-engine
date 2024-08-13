#pragma once

#include <vector>
#include <string>
#include "Texture.hpp"

namespace VoxelEngine
{

class AssetManager
{
public:
	AssetManager();
	~AssetManager();
	Texture& LoadTexture(std::string path, std::string type);
	uint32_t LoadCubemap(std::vector<std::string> faces);

private:
	static std::vector<Texture> m_LoadedTextures;
	uint32_t LoadTextureFromFile(const std::string& fullpath);
	uint32_t LoadTextureFromFile(const std::string& path, const std::string& directory);
};
}; 
