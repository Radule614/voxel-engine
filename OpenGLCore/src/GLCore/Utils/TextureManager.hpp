#pragma once

#include <vector>
#include <string>

namespace GLCore::Utils
{
struct Texture
{
    unsigned int id;
    std::string type;
    std::string path;
};

class TextureManager
{
public:
    TextureManager();
    ~TextureManager();
    Texture &LoadTexture(std::string path, std::string type);
    uint32_t LoadCubemap(std::vector<std::string> faces);

private:
    static std::vector<Texture> m_LoadedTextures;
    uint32_t LoadTextureFromFile(const std::string &fullpath);
    uint32_t LoadTextureFromFile(const std::string &path, const std::string &directory);
};
}; // namespace GLCore::Utils
