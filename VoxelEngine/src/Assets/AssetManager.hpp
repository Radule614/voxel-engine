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
    Texture& LoadHdrTexture(const std::string& path);

    const Model& GetSphereModel() const;
    static std::string GetShaderPath(const std::string& shaderName);

private:
    std::unique_ptr<Model> m_SphereModel;
    std::vector<Texture> m_LoadedTextures;

    static AssetManager* g_AssetManager;
};

}
