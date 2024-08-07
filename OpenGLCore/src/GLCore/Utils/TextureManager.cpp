#include "glpch.hpp"
#include "TextureManager.hpp"

#include <glad/glad.h>
#include <stb_image.h>

namespace GLCore::Utils
{

std::vector<Texture> TextureManager::m_LoadedTextures = {};

TextureManager::TextureManager()
{
}
TextureManager::~TextureManager()
{
}

Texture &TextureManager::LoadTexture(std::string path, std::string type)
{
    for (uint32_t i = 0; i < m_LoadedTextures.size(); i++)
    {
        if (std::strcmp(m_LoadedTextures[i].path.data(), path.c_str()) == 0)
        {
            LOG_INFO("Texture already present: " + m_LoadedTextures[i].path);
            return m_LoadedTextures[i];
        }
    }

    Texture texture;
    texture.id = LoadTextureFromFile(path);
    texture.type = type;
    texture.path = path;

    m_LoadedTextures.push_back(texture);
    return m_LoadedTextures[m_LoadedTextures.size() - 1];
}

uint32_t TextureManager::LoadTextureFromFile(const std::string &fullpath)
{
    uint32_t id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    int32_t width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load((fullpath).c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        LOG_INFO("Failed to load texture");
    }
    stbi_image_free(data);
    LOG_INFO("Loaded: " + fullpath);
    return id;
}

uint32_t TextureManager::LoadTextureFromFile(const std::string &path, const std::string &directory)
{
    return LoadTextureFromFile(directory + "/" + path);
}

uint32_t TextureManager::LoadCubemap(std::vector<std::string> faces)
{
    uint32_t textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    int32_t width, height, nrChannels;
    for (uint32_t i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0,
                         GL_RGB,
                         width,
                         height,
                         0,
                         GL_RGB,
                         GL_UNSIGNED_BYTE,
                         data);
            stbi_image_free(data);
            LOG_INFO("Loaded: " + faces[i]);
        }
        else
        {
            LOG_INFO("Cubemap tex failed to load at path: " + faces[i]);
            stbi_image_free(data);
        }
    }
    return textureID;
}
}; // namespace GLCore::Utils
