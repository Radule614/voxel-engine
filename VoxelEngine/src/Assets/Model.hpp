//
// Created by RadU on 12/11/2025.
//

#pragma once

#include <memory>
#include "tiny_gltf.hpp"
#include "glad/glad.h"

namespace VoxelEngine
{

class Model
{
public:
    explicit Model(tinygltf::Model* model);

    void Bind();
    void Draw();

private:
    void BindMesh(tinygltf::Model& model, tinygltf::Mesh& mesh);
    void BindNodes(tinygltf::Model& model, tinygltf::Node& node);

    void DrawMesh(tinygltf::Model& model, tinygltf::Mesh& mesh);
    void DrawNodes(tinygltf::Model& model,tinygltf::Node& node);

    std::unique_ptr<tinygltf::Model> m_GltfModel;

    GLuint m_Vao;
    std::map<int32_t, GLuint> m_Ebos;
};

}
