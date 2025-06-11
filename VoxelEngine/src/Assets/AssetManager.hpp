#pragma once

#include <vector>
#include <string>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include "Texture.hpp"
#include "Model.hpp"

namespace VoxelEngine
{

class AssetManager
{
public:
    static void Init();
    static void Shutdown();
    static AssetManager& Instance();

    Texture& LoadTexture(const std::string& path, const std::string& type);
    uint32_t LoadCubemap(const std::vector<std::string>& faces);

    Model* LoadModel(const std::string& path);

private:
    static std::vector<Texture> m_LoadedTextures;

    uint32_t LoadTextureFromFile(const std::string& fullpath, int32_t type, bool flip);
    uint32_t LoadTextureFromFile(const std::string& path, int32_t type, bool flip, const std::string& directory);

    void ProcessNode(const aiNode* node, const aiScene* scene, Model& model, const std::string& directory);
    Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene, std::string directory);
    std::vector<Texture> LoadMaterialTextures(const aiMaterial* mat,
                                              aiTextureType type,
                                              const std::string& typeName,
                                              const std::string& directory);
};

inline AssetManager* g_AssetManager = nullptr;

};
