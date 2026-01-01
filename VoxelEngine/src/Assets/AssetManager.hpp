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
    AssetManager();
    ~AssetManager();

    static void Init();
    static void Shutdown();
    static AssetManager& Instance();

    Model* LoadModel(std::string filename);

    Texture& LoadTexture(const std::string& path, const std::string& type);

    const Model& GetSphereModel() const;
    static std::string GetShaderPath(const std::string& shaderName);

private:
    uint32_t LoadTextureFromFile(const std::string& fullpath, int32_t type, bool flip);

    std::unique_ptr<Model> m_SphereModel;

    static AssetManager* g_AssetManager;
    static std::vector<Texture> m_LoadedTextures;
};

}
