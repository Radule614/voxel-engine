//
// Created by RadU on 12/11/2025.
//

#pragma once

#include "GLCoreUtils.hpp"
#include "Gltf/Model.hpp"
#include "Texture.hpp"

namespace VoxelEngine
{

class AssetManager
{
public:
    static void Init();
    static void Shutdown();
    static AssetManager& Instance();

    Texture& LoadTexture(const std::string& path, const std::string& type);

    Model* LoadModel(std::string filename);

private:
    uint32_t LoadTextureFromFile(const std::string& fullpath, int32_t type, bool flip);

    static AssetManager* g_AssetManager;
    static std::vector<Texture> m_LoadedTextures;
};

}
