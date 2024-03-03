#include "VoxelLayer.hpp"
#include "VoxelMeshBuilder.hpp"
#include <vector>

#include "Vertex.hpp"
#include "World.hpp"

using namespace GLCore;
using namespace GLCore::Utils;

namespace Terrain
{
VoxelLayer::VoxelLayer() : m_CameraController(45.0f, 16.0f / 9.0f, 80.0f), m_RenderMetadata({})
{
    //TEMP
    m_TextureManager = TextureManager();
}

VoxelLayer::~VoxelLayer()
{
}

void VoxelLayer::OnAttach()
{
    EnableGLDebugging();
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_FRONT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    m_Shader = Shader::FromGLSLTextFiles("VoxelEngine/assets/shaders/default.vert.glsl",
                                         "VoxelEngine/assets/shaders/default.frag.glsl");
    m_TextureAtlas = m_TextureManager.LoadTexture("VoxelEngine/assets/textures/atlas.png", "texture_diffuse");

    m_World = World();
    m_World.Generate();
    auto &chunkMap = m_World.GetChunkMap();
    for (auto it = chunkMap.cbegin(); it != chunkMap.cend(); ++it)
    {
        std::vector<Vertex> vertices = it->second.GetMesh();
        glm::vec3 pos = it->second.GetPosition();
        ChunkRenderMetadata metadata = {};
        glCreateVertexArrays(1, &metadata.VertexArray);
        glBindVertexArray(metadata.VertexArray);

        glCreateBuffers(1, &metadata.VertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, metadata.VertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(offsetof(Vertex, Vertex::Normal)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(offsetof(Vertex, Vertex::Texture)));

        std::vector<uint32_t> indices = {};
        size_t faceCount = vertices.size() / 4;
        for (size_t i = 0; i < faceCount; ++i)
        {
            indices.push_back(i * 4 + 0);
            indices.push_back(i * 4 + 1);
            indices.push_back(i * 4 + 2);
            indices.push_back(i * 4 + 2);
            indices.push_back(i * 4 + 3);
            indices.push_back(i * 4 + 0);
        }
        glCreateBuffers(1, &metadata.IndexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, metadata.IndexBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), &indices[0], GL_STATIC_DRAW);

        glBindVertexArray(0);

        metadata.Indices = indices;
        metadata.ModelMatrix = it->second.GetModelMatrix();
        m_RenderMetadata.insert({it->second.GetPosition(), metadata});
    }
}

void VoxelLayer::OnDetach()
{
    for (auto it = m_RenderMetadata.begin(); it != m_RenderMetadata.end(); ++it)
    {
        ChunkRenderMetadata &metadata = it->second;
        glDeleteVertexArrays(1, &metadata.VertexArray);
        glDeleteBuffers(1, &metadata.VertexBuffer);
        glDeleteBuffers(1, &metadata.IndexBuffer);
        metadata.Indices.clear();
    }
    m_RenderMetadata.clear();
}


void VoxelLayer::OnEvent(GLCore::Event &event)
{
    m_CameraController.OnEvent(event);
}

void VoxelLayer::OnUpdate(Timestep ts)
{
    m_CameraController.OnUpdate(ts);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(m_Shader->GetRendererID());

    for (auto it = m_RenderMetadata.cbegin(); it != m_RenderMetadata.cend(); ++it)
    {
        const ChunkRenderMetadata &metadata = it->second;

        auto &viewMatrix = m_CameraController.GetCamera().GetViewProjectionMatrix();
        int location = glGetUniformLocation(m_Shader->GetRendererID(), "u_ViewProjection");
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(viewMatrix));
        location = glGetUniformLocation(m_Shader->GetRendererID(), "u_Model");
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(metadata.ModelMatrix));

        glActiveTexture(GL_TEXTURE0);
        location = glGetUniformLocation(m_Shader->GetRendererID(), "u_Atlas");
        glUniform1i(location, 0);
        glBindTexture(GL_TEXTURE_2D, m_TextureAtlas.id);

        glBindVertexArray(metadata.VertexArray);
        glDrawElements(GL_TRIANGLES, metadata.Indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}

void VoxelLayer::OnImGuiRender()
{
}
}; // namespace Terrain
