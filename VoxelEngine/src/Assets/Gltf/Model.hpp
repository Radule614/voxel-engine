//
// Created by RadU on 12/11/2025.
//

#pragma once

#include <memory>

#include "../RenderPrimitive.hpp"
#include "tiny_gltf.hpp"
#include "glad/glad.h"

namespace VoxelEngine
{

class Model
{
public:
    explicit Model(tinygltf::Model* model);
    ~Model();

    void Load();

    const tinygltf::Model& GetRawModel() const;
    const std::vector<RenderPrimitive>& GetMeshPrimitives(int32_t meshIndex) const;

private:
    void LoadNodes(const tinygltf::Node& node);
    void LoadMesh(const tinygltf::Mesh& mesh, int32_t meshIndex);

    GLuint LoadBuffer(int32_t bufferViewIndex);
    GLuint LoadTexture(int32_t textureIndex);

    std::unique_ptr<tinygltf::Model> m_GltfModel;

    std::map<int32_t, GLuint> m_AllocatedBuffers;
    std::map<int32_t, GLuint> m_Textures;
    std::map<int32_t, std::vector<RenderPrimitive> > m_MeshPrimitiveMap;
};

}
