//
// Created by RadU on 12/11/2025.
//

#include <ranges>
#include "Model.hpp"
#include "GLCoreUtils.hpp"
#include "../Utils/Utils.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace VoxelEngine
{

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

static std::map<std::string, GLuint> VertexAttributeIndexMap = {
    {"POSITION", 0},
    {"NORMAL", 1},
    {"TEXCOORD_0", 2},
};

Model::Model(tinygltf::Model* model) : m_GltfModel(model) { Load(); }

Model::~Model()
{
    for (auto glBuffer: m_AllocatedBuffers | std::views::values)
        glDeleteBuffers(1, &glBuffer);

    for (const auto& primitives: m_MeshPrimitiveMap | std::views::values)
        for (auto primitive: primitives)
            glDeleteVertexArrays(1, &primitive.Vao);
}

void Model::Load()
{
    tinygltf::Model& model = *m_GltfModel;

    const tinygltf::Scene& scene = model.scenes[model.defaultScene];
    for (const int32_t node: scene.nodes)
    {
        GLCORE_ASSERT((node >= 0) && (node < model.nodes.size()));
        LoadNodes(model.nodes[node]);
    }
}

void Model::Draw(const GLCore::Utils::Shader& shader, glm::mat4 modelMatrix) const
{
    tinygltf::Model& model = *m_GltfModel;

    const tinygltf::Scene& scene = model.scenes[model.defaultScene];
    for (const int32_t node: scene.nodes)
        DrawNodes(shader, model.nodes[node], modelMatrix);
}

void Model::DrawMesh(const GLCore::Utils::Shader& shader,
                     tinygltf::Mesh& mesh,
                     int32_t meshIndex,
                     glm::mat4 modelMatrix) const
{
    std::vector<RenderPrimitive> renderPrimitives = m_MeshPrimitiveMap.at(meshIndex);
    GLCORE_ASSERT(renderPrimitives.size() == mesh.primitives.size());

    for (size_t i = 0; i < mesh.primitives.size(); ++i)
    {
        auto [Vao, Mode, IndexCount, IndexType, Material] = renderPrimitives[i];

        glBindVertexArray(Vao);

        shader.SetModel(modelMatrix);

        shader.SetVec4("u_BaseColorFactor", Material.BaseColorFactor);

        glDrawElements(Mode, IndexCount, IndexType, nullptr);
    }

    glBindVertexArray(0);
}

glm::mat4 Model::GetLocalTransformMatrix(const tinygltf::Node& node)
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

void Model::DrawNodes(const GLCore::Utils::Shader& shader, tinygltf::Node& node, glm::mat4 modelMatrix) const
{
    tinygltf::Model& model = *m_GltfModel;

    glm::mat4 localMatrix = GetLocalTransformMatrix(node);
    modelMatrix = modelMatrix * localMatrix;

    if ((node.mesh >= 0) && (node.mesh < model.meshes.size()))
        DrawMesh(shader, model.meshes[node.mesh], node.mesh, modelMatrix);

    for (const int32_t childNode: node.children)
        DrawNodes(shader, model.nodes[childNode], modelMatrix);
}

GLuint Model::GetBuffer(int32_t bufferViewIndex)
{
    if (m_AllocatedBuffers.contains(bufferViewIndex))
        return m_AllocatedBuffers[bufferViewIndex];

    const tinygltf::Model& model = *m_GltfModel;

    const tinygltf::BufferView& bufferView = model.bufferViews[bufferViewIndex];
    const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];
    LOG_INFO("bufferview.target: {0}", bufferView.target)
    LOG_INFO("buffer.data.size = {0}, bufferview.byteOffset = {1}", buffer.data.size(), bufferView.byteOffset)

    GLuint glBuffer;
    glGenBuffers(1, &glBuffer);
    glBindBuffer(bufferView.target, glBuffer);
    glBufferData(bufferView.target,
                 bufferView.byteLength,
                 &buffer.data.at(0) + bufferView.byteOffset,
                 GL_STATIC_DRAW);

    m_AllocatedBuffers[bufferViewIndex] = glBuffer;

    return glBuffer;
}

void Model::LoadNodes(tinygltf::Node& node)
{
    tinygltf::Model& model = *m_GltfModel;

    if ((node.mesh >= 0) && (node.mesh < model.meshes.size()))
        LoadMesh(model.meshes[node.mesh], node.mesh);

    for (const int32_t childNode: node.children)
    {
        GLCORE_ASSERT((node.children[i] >= 0) && (node.children[i] < model.nodes.size()));
        LoadNodes(model.nodes[childNode]);
    }
}

void Model::LoadMesh(tinygltf::Mesh& mesh, int32_t meshIndex)
{
    const tinygltf::Model& model = *m_GltfModel;

    std::vector<RenderPrimitive> vaoArray{};

    for (const auto& primitive: mesh.primitives)
    {
        GLuint vao;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        for (const auto& [primitiveType, accessorIndex]: primitive.attributes)
        {
            tinygltf::Accessor accessor = model.accessors[accessorIndex];
            int byteStride = accessor.ByteStride(model.bufferViews[accessor.bufferView]);
            glBindBuffer(GL_ARRAY_BUFFER, GetBuffer(accessor.bufferView));

            int size = 1;
            if (accessor.type != TINYGLTF_TYPE_SCALAR) { size = accessor.type; }

            if (VertexAttributeIndexMap.contains(primitiveType))
            {
                const GLuint index = VertexAttributeIndexMap[primitiveType];

                const auto& offset = BUFFER_OFFSET(accessor.byteOffset);
                const auto isNormalized = accessor.normalized ? GL_TRUE : GL_FALSE;

                glEnableVertexAttribArray(index);
                glVertexAttribPointer(index, size, accessor.componentType, isNormalized, byteStride, offset);
            }
            else
                LOG_WARN("vaa missing: {0}", primitiveType);
        }

        tinygltf::Accessor indexBufferAccessor = model.accessors[primitive.indices];
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GetBuffer(indexBufferAccessor.bufferView));

        RenderPrimitive renderPrimitive{};
        renderPrimitive.Vao = vao;
        renderPrimitive.Mode = primitive.mode;
        renderPrimitive.IndexCount = indexBufferAccessor.count;
        renderPrimitive.IndexType = indexBufferAccessor.componentType;

        if (primitive.material >= 0)
        {
            const tinygltf::Material& material = model.materials[primitive.material];
            const std::vector<double>& color = material.pbrMetallicRoughness.baseColorFactor;

            renderPrimitive.Material.BaseColorFactor = glm::vec4(color[0], color[1], color[2], color[3]);
        }

        glBindVertexArray(0);

        vaoArray.push_back(renderPrimitive);
    }

    m_MeshPrimitiveMap[meshIndex] = vaoArray;
}

}
