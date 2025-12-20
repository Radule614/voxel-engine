//
// Created by RadU on 12/13/2025.
//

#include "Model.hpp"
#include "GLCoreUtils.hpp"
#include "../../Utils/Utils.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace GLCore;
using namespace GLCore::Utils;

namespace VoxelEngine
{

static glm::mat4 GetLocalTransformMatrix(const tinygltf::Node& node);
static void SetShaderMaterial(const Shader& shader, const Material& material);

void Model::DrawNodes(const Shader& shader,
                      glm::mat4 modelMatrix,
                      const tinygltf::Node& node) const
{
    const tinygltf::Model& model = *m_GltfModel;

    modelMatrix = modelMatrix * GetLocalTransformMatrix(node);

    if (node.mesh >= 0 && node.mesh < model.meshes.size())
        DrawMesh(shader, modelMatrix, model.meshes[node.mesh], node.mesh);

    for (const int32_t childNode: node.children)
        DrawNodes(shader, modelMatrix, model.nodes[childNode]);
}

void Model::Draw(const Shader& shader, glm::mat4 modelMatrix) const
{
    const tinygltf::Model& model = *m_GltfModel;
    const tinygltf::Scene& scene = model.scenes[model.defaultScene];

    for (const int32_t node: scene.nodes)
        DrawNodes(shader, modelMatrix, model.nodes[node]);
}

void Model::DrawMesh(const Shader& shader,
                     glm::mat4 modelMatrix,
                     const tinygltf::Mesh& mesh,
                     const int32_t meshIndex) const
{
    std::vector<RenderPrimitive> renderPrimitives = m_MeshPrimitiveMap.at(meshIndex);
    GLCORE_ASSERT(renderPrimitives.size() == mesh.primitives.size());

    for (size_t i = 0; i < mesh.primitives.size(); ++i)
    {
        auto [Vao, Mode, IndexCount, IndexType, Material] = renderPrimitives[i];

        glBindVertexArray(Vao);

        shader.SetModel(modelMatrix);

        SetShaderMaterial(shader, Material);

        glDrawElements(Mode, IndexCount, IndexType, nullptr);
    }

    glBindVertexArray(0);
}

static void SetShaderMaterial(const Shader& shader, const Material& material)
{
    if (material.TextureId > 0)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, material.TextureId);

        shader.SetBool("u_HasAlbedoTexture", true);
        shader.SetInt("u_Albedo", 0);
    }
    else
    {
        shader.SetBool("u_HasAlbedoTexture", false);
        shader.SetVec4("u_AlbedoColor", material.BaseColorFactor);
    }
}

static glm::mat4 GetLocalTransformMatrix(const tinygltf::Node& node)
{
    if (!node.matrix.empty())
        return VectorToMat4(node.matrix);

    glm::vec3 T = node.translation.size() == 3
                      ? glm::vec3(node.translation[0], node.translation[1], node.translation[2])
                      : glm::vec3(0.0f);

    glm::quat R = node.rotation.size() == 4
                      ? glm::quat(node.rotation[3], node.rotation[0], node.rotation[1], node.rotation[2])
                      : glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

    glm::vec3 S = node.scale.size() == 3
                      ? glm::vec3(node.scale[0], node.scale[1], node.scale[2])
                      : glm::vec3(1.0f);

    return glm::translate(glm::mat4(1.0f), T) * glm::mat4_cast(R) * glm::scale(glm::mat4(1.0f), S);
}

}
