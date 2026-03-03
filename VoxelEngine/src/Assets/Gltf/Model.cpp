//
// Created by RadU on 12/11/2025.
//

#include "Model.hpp"
#include "GLCoreUtils.hpp"
#include <ranges>
#include <glm/glm.hpp>

#include "glm/gtc/type_ptr.hpp"

using namespace GLCore;
using namespace GLCore::Utils;

namespace VoxelEngine
{

static std::map<std::string, GLuint> VertexAttributeIndexMap = {
    {"POSITION", 0},
    {"NORMAL", 1},
    {"TEXCOORD_0", 2},
    {"TANGENT", 3},
    {"JOINTS_0", 4},
    {"WEIGHTS_0", 5}
};

static glm::vec4 Vec4FromVector(std::vector<double_t> vector);

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
        GLCORE_ASSERT((node >= 0) && (node < model.nodes.size()), "node vector length invalid");

        LoadNodes(model.nodes[node]);
    }

    LoadAnimations();
    LoadSkins();
}

const tinygltf::Model& Model::GetRawModel() const { return *m_GltfModel; }

const std::vector<RenderPrimitive>& Model::GetMeshPrimitives(const int32_t meshIndex) const
{
    return m_MeshPrimitiveMap.at(meshIndex);
}

const std::vector<Animation>& Model::GetAnimations() const { return m_Animations; }

const std::vector<Skin>& Model::GetSkins() const { return m_Skins; }

void Model::LoadNodes(const tinygltf::Node& node)
{
    const tinygltf::Model& model = *m_GltfModel;

    if (node.mesh >= 0 && node.mesh < model.meshes.size())
        LoadMesh(model.meshes[node.mesh], node.mesh);

    for (const int32_t childNode: node.children)
    {
        GLCORE_ASSERT(node.children[childNode] >= 0 && node.children[childNode] < model.nodes.size(), "node children length invalid");

        LoadNodes(model.nodes[childNode]);
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

void Model::LoadMesh(const tinygltf::Mesh& mesh, const int32_t meshIndex)
{
    const tinygltf::Model& model = *m_GltfModel;
    std::vector<RenderPrimitive> renderPrimitives{};

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

            const auto offset = (void*) accessor.byteOffset;
            const int32_t isNormalized = accessor.normalized ? GL_TRUE : GL_FALSE;
            const int32_t size = tinygltf::GetNumComponentsInType(accessor.type);
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
        renderPrimitive.IndexOffset = (void*) indexBufferAccessor.byteOffset;

        if (primitive.material >= 0)
        {
            const tinygltf::Material& material = model.materials[primitive.material];

            renderPrimitive.Material.AlbedoFactor = Vec4FromVector(material.pbrMetallicRoughness.baseColorFactor);
            renderPrimitive.Material.AlbedoTextureId =
                    LoadTexture(material.pbrMetallicRoughness.baseColorTexture.index);

            renderPrimitive.Material.MetallicFactor = material.pbrMetallicRoughness.metallicFactor;
            renderPrimitive.Material.RoughnessFactor = material.pbrMetallicRoughness.roughnessFactor;
            renderPrimitive.Material.MetallicRoughnessTextureId =
                    LoadTexture(material.pbrMetallicRoughness.metallicRoughnessTexture.index);

            renderPrimitive.Material.AmbientOcclusionTextureId = LoadTexture(material.occlusionTexture.index);
            renderPrimitive.Material.AmbientOcclusionStrength = material.occlusionTexture.strength;

            renderPrimitive.Material.NormalTextureId = LoadTexture(material.normalTexture.index);
            renderPrimitive.Material.NormalScale = material.normalTexture.scale;
        }

        glBindVertexArray(0);

        renderPrimitives.push_back(renderPrimitive);
    }

    m_MeshPrimitiveMap[meshIndex] = renderPrimitives;
}

GLuint Model::LoadTexture(const int32_t textureIndex)
{
    if (textureIndex < 0)
        return 0;

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

    GLenum format = GL_RGBA;

    if (image.component == 1) format = GL_RED;
    else if (image.component == 2) format = GL_RG;
    else if (image.component == 3) format = GL_RGB;
    else if (image.component == 4) format = GL_RGBA;
    else
        GLCORE_ASSERT(false, "");

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

void Model::LoadAnimations()
{
    const tinygltf::Model& model = *m_GltfModel;

    if (model.animations.empty())
        return;

    const auto& buffers = model.buffers;
    const auto& bufferViews = model.bufferViews;
    const auto& accessors = model.accessors;

    for (const auto& gltfAnimation: model.animations)
    {
        Animation animation;
        animation.Name = gltfAnimation.name;

        const auto& channels = gltfAnimation.channels;
        const auto& samplers = gltfAnimation.samplers;

        for (const tinygltf::AnimationChannel& channel: channels)
        {
            AnimationTrack track;

            const tinygltf::AnimationSampler& sampler = samplers[channel.sampler];

            // Target

            track.Target = Translation;
            if (channel.target_path == "rotation")
                track.Target = Rotation;
            else if (channel.target_path == "scale")
                track.Target = Scale;
            else if (channel.target_path == "weights")
                track.Target = Weights;

            // Interpolation

            track.Interpolation = Linear;
            if (sampler.interpolation == "STEP")
                track.Interpolation = Step;
            else if (sampler.interpolation == "CUBICSPLINE")
                track.Interpolation = CubicSpline;

            // Inputs

            const tinygltf::Accessor& inputAccessor = accessors[sampler.input];
            const tinygltf::BufferView& inputBufferView = bufferViews[inputAccessor.bufferView];
            const tinygltf::Buffer& inputBuffer = buffers[inputBufferView.buffer];

            const uint8_t* inputData =
                    inputBuffer.data.data() + inputBufferView.byteOffset + inputAccessor.byteOffset;

            track.Times.resize(inputAccessor.count);

            for (size_t i = 0; i < inputAccessor.count; ++i)
            {
                const size_t stride = inputBufferView.byteStride > 0
                                          ? inputBufferView.byteStride
                                          : sizeof(float);
                track.Times[i] = *reinterpret_cast<const float*>(inputData + i * stride);
            }

            // Outputs

            const tinygltf::Accessor& outputAccessor = accessors[sampler.output];
            const tinygltf::BufferView& outputBufferView = bufferViews[outputAccessor.bufferView];
            const tinygltf::Buffer& outputBuffer = buffers[outputBufferView.buffer];

            const uint8_t* outputData =
                    outputBuffer.data.data() + outputBufferView.byteOffset + outputAccessor.byteOffset;

            if (track.Target == Translation || track.Target == Scale)
            {
                track.VectorValues.resize(outputAccessor.count);

                for (size_t i = 0; i < outputAccessor.count; ++i)
                {
                    const size_t stride = outputBufferView.byteStride > 0
                                              ? outputBufferView.byteStride
                                              : sizeof(float) * 3;
                    const auto* fptr = reinterpret_cast<const float*>(outputData + i * stride);
                    track.VectorValues[i] = glm::vec3(fptr[0], fptr[1], fptr[2]);
                }
            }

            if (track.Target == Rotation)
            {
                track.QuatValues.resize(outputAccessor.count);

                for (size_t i = 0; i < outputAccessor.count; ++i)
                {
                    const size_t stride = outputBufferView.byteStride > 0
                                              ? outputBufferView.byteStride
                                              : sizeof(float) * 4;
                    const auto* fptr = reinterpret_cast<const float*>(outputData + i * stride);
                    track.QuatValues[i] = glm::normalize(glm::quat(fptr[3], fptr[0], fptr[1], fptr[2]));
                }
            }

            animation.NodeAnimations[channel.target_node].Tracks.emplace_back(track);
        }

        m_Animations.emplace_back(animation);
    }
}

void Model::LoadSkins()
{
    const tinygltf::Model& model = *m_GltfModel;

    if (model.skins.empty())
        return;

    int32_t skinIndex = 0;
    for (const auto& gltfSkin: model.skins)
    {
        const tinygltf::Accessor& accessor = model.accessors[gltfSkin.inverseBindMatrices];
        const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
        const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

        Skin skin{};
        skin.InverseBindMatrices.resize(accessor.count);
        skin.JointIndexes = gltfSkin.joints;

        const uint8_t* data = buffer.data.data() + bufferView.byteOffset + accessor.byteOffset;

        for (size_t i = 0; i < accessor.count; ++i)
        {
            const size_t stride = bufferView.byteStride > 0 ? bufferView.byteStride : sizeof(float_t) * 16;
            skin.InverseBindMatrices[i] = glm::make_mat4(reinterpret_cast<const float_t*>(data + i * stride));
        }

        for (int32_t i = 0; i < model.nodes.size(); ++i)
        {
            if (model.nodes[i].skin == skinIndex)
                skin.RootIndexes.push_back(i);
        }

        m_Skins.emplace_back(skin);

        ++skinIndex;
    }
}

static glm::vec4 Vec4FromVector(std::vector<double_t> vector)
{
    if (vector.size() != 4)
        return glm::vec4(0.0f);

    return {vector[0], vector[1], vector[2], vector[3]};
}

}
