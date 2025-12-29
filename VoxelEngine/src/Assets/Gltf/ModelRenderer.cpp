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
    shader.Set<glm::vec4>("u_AlbedoFactor", material.AlbedoFactor);
    if (material.AlbedoTextureId > 0)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, material.AlbedoTextureId);

        shader.Set<bool>("u_HasAlbedoTexture", true);
        shader.Set<int32_t>("u_AlbedoTexture", 0);
    }
    else shader.Set<bool>("u_HasAlbedoTexture", false);

    shader.Set<float_t>("u_MetallicFactor", material.MetallicFactor);
    shader.Set<float_t>("u_RoughnessFactor", material.RoughnessFactor);
    if (material.MetallicRoughnessTextureId > 0)
    {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, material.MetallicRoughnessTextureId);

        shader.Set<bool>("u_HasMetallicRoughnessTexture", true);
        shader.Set<int32_t>("u_MetallicRoughnessTexture", 1);
    }
    else shader.Set<bool>("u_HasMetallicRoughnessTexture", false);

    if (material.AmbientOcclusionTextureId > 0)
    {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, material.AmbientOcclusionTextureId);

        shader.Set<bool>("u_HasAmbientOcclusionTexture", true);
        shader.Set<int32_t>("u_AmbientOcclusionTexture", 2);
    }
    else shader.Set<bool>("u_HasAmbientOcclusionTexture", false);

    if (material.NormalTextureId > 0)
    {
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, material.NormalTextureId);

        shader.Set<bool>("u_HasNormalTexture", true);
        shader.Set<int32_t>("u_NormalTexture", 3);
    }
    else shader.Set<bool>("u_HasNormalTexture", false);
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
