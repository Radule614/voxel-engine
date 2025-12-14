//
// Created by RadU on 12/11/2025.
//

#include "Model.hpp"
#include "GLCoreUtils.hpp"
#include "stb_image.hpp"
#include <ranges>
#include <glm/glm.hpp>

using namespace GLCore;
using namespace GLCore::Utils;

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

    for (auto textureId: m_Textures | std::views::values)
        glDeleteTextures(1, &textureId);
}

void Model::Load()
{
    const tinygltf::Model& model = *m_GltfModel;

    const tinygltf::Scene& scene = model.scenes[model.defaultScene];
    for (const int32_t node: scene.nodes)
    {
        GLCORE_ASSERT((node >= 0) && (node < model.nodes.size()));
        LoadNodes(model.nodes[node]);
    }
}

GLuint Model::LoadBuffer(const int32_t bufferViewIndex)
{
    if (m_AllocatedBuffers.contains(bufferViewIndex))
        return m_AllocatedBuffers[bufferViewIndex];

    const tinygltf::Model& model = *m_GltfModel;

    const tinygltf::BufferView& bufferView = model.bufferViews[bufferViewIndex];
    const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

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

void Model::LoadNodes(const tinygltf::Node& node)
{
    const tinygltf::Model& model = *m_GltfModel;

    if (node.mesh >= 0 && node.mesh < model.meshes.size())
        LoadMesh(model.meshes[node.mesh], node.mesh);

    for (const int32_t childNode: node.children)
    {
        GLCORE_ASSERT(node.children[i] >= 0 && node.children[i] < model.nodes.size());

        LoadNodes(model.nodes[childNode]);
    }
}

void Model::LoadMesh(const tinygltf::Mesh& mesh, const int32_t meshIndex)
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
            if (!VertexAttributeIndexMap.contains(primitiveType))
                continue;

            tinygltf::Accessor accessor = model.accessors[accessorIndex];
            glBindBuffer(GL_ARRAY_BUFFER, LoadBuffer(accessor.bufferView));

            const GLuint index = VertexAttributeIndexMap[primitiveType];

            const auto& offset = BUFFER_OFFSET(accessor.byteOffset);
            const int32_t isNormalized = accessor.normalized ? GL_TRUE : GL_FALSE;
            const int32_t size = accessor.type != TINYGLTF_TYPE_SCALAR ? accessor.type : 1;
            const int32_t byteStride = accessor.ByteStride(model.bufferViews[accessor.bufferView]);

            glEnableVertexAttribArray(index);
            glVertexAttribPointer(index, size, accessor.componentType, isNormalized, byteStride, offset);
        }

        const tinygltf::Accessor indexBufferAccessor = model.accessors[primitive.indices];
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, LoadBuffer(indexBufferAccessor.bufferView));

        RenderPrimitive renderPrimitive{};
        renderPrimitive.Vao = vao;
        renderPrimitive.Mode = primitive.mode;
        renderPrimitive.IndexCount = indexBufferAccessor.count;
        renderPrimitive.IndexType = indexBufferAccessor.componentType;

        if (primitive.material >= 0)
        {
            const tinygltf::Material& material = model.materials[primitive.material];
            const std::vector<double_t>& color = material.pbrMetallicRoughness.baseColorFactor;

            if (color.size() == 4)
                renderPrimitive.Material.BaseColorFactor = glm::vec4(color[0], color[1], color[2], color[3]);

            const int32_t textureIndex = material.pbrMetallicRoughness.baseColorTexture.index;
            if (textureIndex >= 0)
                renderPrimitive.Material.TextureId = LoadTexture(textureIndex);
        }

        glBindVertexArray(0);

        vaoArray.push_back(renderPrimitive);
    }

    m_MeshPrimitiveMap[meshIndex] = vaoArray;
}

GLuint Model::LoadTexture(const int32_t textureIndex)
{
    if (m_Textures.contains(textureIndex))
        return m_Textures[textureIndex];

    const tinygltf::Model& model = *m_GltfModel;
    const tinygltf::Texture& texture = model.textures[textureIndex];
    const tinygltf::Image& image = model.images[texture.source];

    uint32_t textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    GLenum format = GL_RGBA;

    if (image.component == 1) format = GL_RED;
    else if (image.component == 2) format = GL_RG;
    else if (image.component == 3) format = GL_RGB;
    else if (image.component == 4) format = GL_RGBA;
    else
        assert(false);

    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 format,
                 image.width,
                 image.height,
                 0,
                 format,
                 image.pixel_type,
                 &image.image[0]);
    glGenerateMipmap(GL_TEXTURE_2D);

    m_Textures[textureIndex] = textureId;

    return textureId;
}

}
