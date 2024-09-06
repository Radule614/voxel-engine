#include "GLCore.hpp"
#include "AssetManager.hpp"
#include "stb_image/stb_image.h"
#include <glad/glad.h>

namespace VoxelEngine
{

std::vector<Texture> AssetManager::m_LoadedTextures = {};

AssetManager::AssetManager()
{
}
AssetManager::~AssetManager()
{
}

Texture& AssetManager::LoadTexture(std::string path, std::string type)
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
	texture.id = LoadTextureFromFile(path, GL_RGBA, true);
	texture.type = type;
	texture.path = path;

	m_LoadedTextures.push_back(texture);
	return m_LoadedTextures[m_LoadedTextures.size() - 1];
}

Model* AssetManager::LoadModel(std::string path)
{
	Assimp::Importer import;
	const aiScene * scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		LOG_INFO("ERROR::ASSIMP::{0}", import.GetErrorString());
		return nullptr;
	}
	std::string directory = path.substr(0, path.find_last_of('/'));
	Model* model = new Model();
	ProcessNode(scene->mRootNode, scene, *model, directory);
	return model;
}

uint32_t AssetManager::LoadCubemap(std::vector<std::string> faces)
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
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
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

uint32_t AssetManager::LoadTextureFromFile(const std::string& fullpath, int32_t type, bool flip)
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
	else
	{
		LOG_INFO("Failed to load texture");
	}
	stbi_image_free(data);
	LOG_INFO("Loaded: " + fullpath);
	return id;
}

uint32_t AssetManager::LoadTextureFromFile(const std::string& path, int32_t type, bool flip, const std::string& directory)
{
	return LoadTextureFromFile(directory + "/" + path, flip, type);
}

void AssetManager::ProcessNode(aiNode* node, const aiScene* scene, Model& model, std::string directory)
{
	for (size_t i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		model.Meshes.push_back(ProcessMesh(mesh, scene, directory));
	}
	for (size_t i = 0; i < node->mNumChildren; i++)
		ProcessNode(node->mChildren[i], scene, model, directory);
}

Mesh AssetManager::ProcessMesh(aiMesh* mesh, const aiScene* scene, std::string directory)
{
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	std::vector<Texture> textures;
	for (size_t i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex{};
		glm::vec3 vector{};
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;
		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.Normal = vector;
		vector.x = mesh->mTangents[i].x;
		vector.y = mesh->mTangents[i].y;
		vector.z = mesh->mTangents[i].z;
		vertex.Tangent = vector;
		if (mesh->mTextureCoords[0])
		{
			glm::vec2 vec{};
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
		}
		else
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		vertices.push_back(vertex);
	}
	for (size_t i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (size_t j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		std::vector<Texture> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "Diffuse", directory);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		std::vector<Texture> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, "Specular", directory);
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		std::vector<Texture> normalMaps = LoadMaterialTextures(material, aiTextureType_HEIGHT, "Normal", directory);
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
	}
	return Mesh(vertices, indices, textures);
}

std::vector<Texture> AssetManager::LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName, std::string directory)
{
	std::vector<Texture> textures;
	for (size_t i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		bool skip = false;
		std::string fullPath = directory + "/" + str.C_Str();
		for (size_t j = 0; j < m_LoadedTextures.size(); j++)
		{
			if (std::strcmp(m_LoadedTextures[j].path.data(), fullPath.c_str()) == 0)
			{
				textures.push_back(m_LoadedTextures[j]);
				skip = true;
				break;
			}
		}
		if (!skip)
		{
			Texture texture;
			texture.id = LoadTextureFromFile(fullPath, GL_RGB, false);
			texture.type = typeName;
			texture.path = fullPath;
			textures.push_back(texture);
			m_LoadedTextures.push_back(texture);
		}
	}
	return textures;
}

};
