#pragma once
#include "Mesh.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h> 

namespace VoxelEngine
{

class Model
{
public:
    Model(const char* path);
    std::vector<Mesh>& GetMeshes();

private:
    std::vector<Mesh> m_Meshes;
    std::string m_Directory;

    void LoadModel(std::string path);
    void ProcessNode(aiNode* node, const aiScene* scene);
    Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};

}