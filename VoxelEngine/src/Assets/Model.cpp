//
// Created by RadU on 12/11/2025.
//

#include "Model.hpp"
#include "GLCoreUtils.hpp"

namespace VoxelEngine
{

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

Model::Model(tinygltf::Model* model) : m_GltfModel(model)
{
}

void Model::Bind()
{
    tinygltf::Model& model = *m_GltfModel;

    std::map<int, GLuint> vbos;
    glGenVertexArrays(1, &m_Vao);
    glBindVertexArray(m_Vao);

    const tinygltf::Scene& scene = model.scenes[model.defaultScene];
    for (size_t i = 0; i < scene.nodes.size(); ++i)
    {
        assert((scene.nodes[i] >= 0) && (scene.nodes[i] < model.nodes.size()));
        BindNodes(model, model.nodes[scene.nodes[i]]);
    }

    glBindVertexArray(0);
    // cleanup vbos but do not delete index buffers yet
    for (auto it = vbos.cbegin(); it != vbos.cend();)
    {
        tinygltf::BufferView bufferView = model.bufferViews[it->first];
        if (bufferView.target != GL_ELEMENT_ARRAY_BUFFER)
        {
            glDeleteBuffers(1, &vbos[it->first]);
            vbos.erase(it++);
        }
        else { ++it; }
    }
}

void Model::Draw()
{
    tinygltf::Model& model = *m_GltfModel;

    glBindVertexArray(m_Vao);

    const tinygltf::Scene& scene = model.scenes[model.defaultScene];
    for (size_t i = 0; i < scene.nodes.size(); ++i) { DrawNodes(model, model.nodes[scene.nodes[i]]); }

    glBindVertexArray(0);
}

void Model::DrawMesh(tinygltf::Model& model, tinygltf::Mesh& mesh)
{
    for (size_t i = 0; i < mesh.primitives.size(); ++i)
    {
        tinygltf::Primitive primitive = mesh.primitives[i];
        tinygltf::Accessor indexAccessor = model.accessors[primitive.indices];

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Ebos.at(indexAccessor.bufferView));

        glDrawElements(primitive.mode,
                       indexAccessor.count,
                       indexAccessor.componentType,
                       BUFFER_OFFSET(indexAccessor.byteOffset));
    }
}

// recursively draw node and children nodes of model
void Model::DrawNodes(tinygltf::Model& model, tinygltf::Node& node)
{
    if ((node.mesh >= 0) && (node.mesh < model.meshes.size())) { DrawMesh(model, model.meshes[node.mesh]); }
    for (size_t i = 0; i < node.children.size(); i++) { DrawNodes(model, model.nodes[node.children[i]]); }
}

void Model::BindNodes(tinygltf::Model& model, tinygltf::Node& node)
{
    if ((node.mesh >= 0) && (node.mesh < model.meshes.size())) { BindMesh(model, model.meshes[node.mesh]); }

    for (size_t i = 0; i < node.children.size(); i++)
    {
        assert((node.children[i] >= 0) && (node.children[i] < model.nodes.size()));
        BindNodes(model, model.nodes[node.children[i]]);
    }
}

void Model::BindMesh(tinygltf::Model& model, tinygltf::Mesh& mesh)
{
    for (size_t i = 0; i < model.bufferViews.size(); ++i)
    {
        const tinygltf::BufferView& bufferView = model.bufferViews[i];
        if (bufferView.target == 0)
        {
            // TODO impl drawarrays
            LOG_WARN("bufferView.target is zero")

            continue;
        }

        const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];
        LOG_INFO("bufferview.target: {0}", bufferView.target)

        GLuint vbo;
        glGenBuffers(1, &vbo);
        m_Ebos[i] = vbo;
        glBindBuffer(bufferView.target, vbo);

        LOG_INFO("buffer.data.size = {0}, bufferview.byteOffset = {1}", buffer.data.size(), bufferView.byteOffset)

        glBufferData(bufferView.target,
                     bufferView.byteLength,
                     &buffer.data.at(0) + bufferView.byteOffset,
                     GL_STATIC_DRAW);
    }

    for (size_t i = 0; i < mesh.primitives.size(); ++i)
    {
        tinygltf::Primitive primitive = mesh.primitives[i];
        tinygltf::Accessor indexAccessor = model.accessors[primitive.indices];

        for (auto& attrib: primitive.attributes)
        {
            tinygltf::Accessor accessor = model.accessors[attrib.second];
            int byteStride =
                    accessor.ByteStride(model.bufferViews[accessor.bufferView]);
            glBindBuffer(GL_ARRAY_BUFFER, m_Ebos[accessor.bufferView]);

            int size = 1;
            if (accessor.type != TINYGLTF_TYPE_SCALAR) { size = accessor.type; }

            int vaa = -1;
            if (attrib.first.compare("POSITION") == 0) vaa = 0;
            if (attrib.first.compare("NORMAL") == 0) vaa = 1;
            if (attrib.first.compare("TEXCOORD_0") == 0) vaa = 2;
            if (vaa > -1)
            {
                glEnableVertexAttribArray(vaa);
                glVertexAttribPointer(vaa,
                                      size,
                                      accessor.componentType,
                                      accessor.normalized ? GL_TRUE : GL_FALSE,
                                      byteStride,
                                      BUFFER_OFFSET(accessor.byteOffset));
            }
            else
                LOG_WARN("vaa missing: {0}", attrib.first);
        }

        if (!model.materials.empty())
        {
            auto material = model.materials[0];

            auto values = material.values["baseColorFactor"].number_array;
            glm::vec3 color = glm::vec3(values[0], values[1], values[2]);


        }
    }
}

}
