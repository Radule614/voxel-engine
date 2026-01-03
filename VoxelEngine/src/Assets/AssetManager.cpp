//
// Created by RadU on 12/11/2025.
//

#include "AssetManager.hpp"

#include "stb_image.hpp"
#include "tiny_gltf.hpp"

namespace VoxelEngine
{

AssetManager* AssetManager::g_AssetManager = nullptr;
std::vector<Texture> AssetManager::m_LoadedTextures = {};

AssetManager::AssetManager() : m_SphereModel(std::unique_ptr<Model>(LoadModel("assets/models/sphere/Sphere.glb")))
{
}

AssetManager::~AssetManager() = default;

void AssetManager::Init() { g_AssetManager = new AssetManager(); }

void AssetManager::Shutdown()
{
    delete g_AssetManager;
    g_AssetManager = nullptr;
}

AssetManager& AssetManager::Instance() { return *g_AssetManager; }

Texture& AssetManager::LoadTexture(const std::string& path, const std::string& type)
{
    for (auto& m_LoadedTexture: m_LoadedTextures)
    {
        if (std::strcmp(m_LoadedTexture.path.data(), path.c_str()) == 0)
        {
            // LOG_INFO("Texture already present: {0}", m_LoadedTexture.path);

            return m_LoadedTexture;
        }
    }

    Texture texture;
    texture.id = LoadTextureFromFile(path, GL_RGBA, true);
    texture.type = type;
    texture.path = path;

    m_LoadedTextures.push_back(texture);
    return m_LoadedTextures[m_LoadedTextures.size() - 1];
}

Texture& AssetManager::LoadHdrTexture(const std::string& path)
{
    stbi_set_flip_vertically_on_load(true);
    int width, height, nrComponents;
    float* data = stbi_loadf(path.c_str(), &width, &height, &nrComponents, 0);

    Texture texture;
    texture.path = path;
    m_LoadedTextures.push_back(texture);

    if (data)
    {
        glGenTextures(1, &texture.id);
        glBindTexture(GL_TEXTURE_2D, texture.id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        LOG_INFO("Loaded hdr image, path: {}", path);

        stbi_image_free(data);
    }
    else LOG_INFO("Failed to load hdr texture, path: {}", path);

    stbi_set_flip_vertically_on_load(false);

    return m_LoadedTextures[m_LoadedTextures.size() - 1];
}

uint32_t AssetManager::LoadTextureFromFile(const std::string& fullpath, const int32_t type, const bool flip)
{
    uint32_t id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    int32_t width, height, nrChannels;
    stbi_set_flip_vertically_on_load(flip);
    unsigned char* data = stbi_load((fullpath).c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, type, width, height, 0, type, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else { LOG_INFO("Failed to load texture"); }
    stbi_set_flip_vertically_on_load(false);
    stbi_image_free(data);
    LOG_INFO("Loaded texture: {0}", fullpath);
    return id;
}

Model* AssetManager::LoadModel(std::string filename)
{
    auto* gltfModel = new tinygltf::Model();
    tinygltf::TinyGLTF loader;
    std::string err;
    std::string warn;

    bool res = loader.LoadBinaryFromFile(gltfModel, &err, &warn, filename);
    if (!warn.empty()) { LOG_INFO("WARN: {0}", warn) }

    if (!err.empty())
        LOG_WARN("ERR: {0}", err)

    if (!res)
        LOG_ERROR("Failed to load glTF: {0}", filename)
    else
        LOG_INFO("Loaded glTF: {0}", filename)

    GLCORE_ASSERT(gltfModel != nullptr)

    return new Model(gltfModel);
}

const Model& AssetManager::GetSphereModel() const { return *m_SphereModel; }

std::string AssetManager::GetShaderPath(const std::string& shaderName) { return "assets/shaders/" + shaderName; }

}
