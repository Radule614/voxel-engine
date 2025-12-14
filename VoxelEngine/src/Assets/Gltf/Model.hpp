//
// Created by RadU on 12/11/2025.
//

#pragma once

#include <memory>

#include "../RenderPrimitive.hpp"
#include "tiny_gltf.hpp"
#include "glad/glad.h"
#include "GLCore/Utils/Shader.hpp"

namespace VoxelEngine
{

class Model
{
public:
    explicit Model(tinygltf::Model* model);
    ~Model();

    void Load();
    void Draw(const GLCore::Utils::Shader& shader, glm::mat4 modelMatrix) const;

private:
    void LoadNodes(const tinygltf::Node& node);
    void LoadMesh(const tinygltf::Mesh& mesh, int32_t meshIndex);

    void DrawNodes(const GLCore::Utils::Shader& shader, glm::mat4 modelMatrix, const tinygltf::Node& node) const;
    void DrawMesh(const GLCore::Utils::Shader& shader,
                  glm::mat4 modelMatrix,
                  const tinygltf::Mesh& mesh,
                  int32_t meshIndex) const;

    GLuint LoadBuffer(int32_t bufferViewIndex);
    GLuint LoadTexture(int32_t textureIndex);

    std::unique_ptr<tinygltf::Model> m_GltfModel;

    std::map<int32_t, GLuint> m_AllocatedBuffers;
    std::map<int32_t, GLuint> m_Textures;
    std::map<int32_t, std::vector<RenderPrimitive> > m_MeshPrimitiveMap;
};

}
