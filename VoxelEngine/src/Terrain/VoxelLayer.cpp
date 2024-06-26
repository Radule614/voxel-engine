#include "VoxelLayer.hpp"
#include "VoxelMeshBuilder.hpp"
#include <vector>

#include "Vertex.hpp"
#include "World.hpp"

using namespace GLCore;
using namespace GLCore::Utils;

namespace Terrain
{
VoxelLayer::VoxelLayer()
    : m_CameraController(45.0f, 16.0f / 9.0f, 150.0f), m_RenderMetadata({}), m_World(World(m_CameraController)),
      m_TextureManager()
{
    m_CameraController.GetCamera().SetPosition(glm::vec3(0.0f, CHUNK_HEIGHT, 0.0f));
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

    m_World.StartGeneration();
}

void VoxelLayer::OnDetach()
{
    m_World.StartGeneration();
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
    if (event.GetEventType() == EventType::WindowClose)
        m_World.StopGeneration();
}

void VoxelLayer::OnUpdate(Timestep ts)
{
    m_CameraController.OnUpdate(ts);

    glClearColor(0.14f, 0.59f, 0.74f, 0.7f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(m_Shader->GetRendererID());

    CheckChunkRenderQueue();

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

void VoxelLayer::CheckChunkRenderQueue()
{
    auto &worldLock = m_World.GetLock();
    auto &chunks = m_World.GetChangedChunks();
    if (chunks.empty() || !worldLock.try_lock())
        return;
    auto it = chunks.begin();
    while (it != chunks.end())
    {
        std::shared_ptr<Chunk> chunk = *it;
        auto &chunkLock = chunk->GetLock();
        if (!chunkLock.try_lock())
        {
            ++it;
            continue;
        }
        SetupRenderData(chunk);
        it = chunks.erase(it);
        chunkLock.unlock();
    }
    worldLock.unlock();
}

void VoxelLayer::SetupRenderData(std::shared_ptr<Chunk> chunk)
{
    // glm::vec3 p = chunk->GetPosition();
    // LOG_INFO(std::to_string(p.x) + ", " + std::to_string(p.y) + ", " + std::to_string(p.z));

    ChunkRenderMetadata metadata = {};
    auto renderData = m_RenderMetadata.find(chunk->GetPosition());
    if (renderData != m_RenderMetadata.end())
    {
        ChunkRenderMetadata &m = renderData->second;
        glDeleteVertexArrays(1, &m.VertexArray);
        glDeleteBuffers(1, &m.VertexBuffer);
        glDeleteBuffers(1, &m.IndexBuffer);
        m.Indices.clear();
        m_RenderMetadata.erase(chunk->GetPosition());
    }

    std::vector<Vertex> vertices = {};
    vertices.insert(vertices.end(), chunk->GetMesh().begin(), chunk->GetMesh().end());
    vertices.insert(vertices.end(),
                    chunk->GetBorderMesh(VoxelFace::FRONT).begin(),
                    chunk->GetBorderMesh(VoxelFace::FRONT).end());
    vertices.insert(vertices.end(),
                    chunk->GetBorderMesh(VoxelFace::RIGHT).begin(),
                    chunk->GetBorderMesh(VoxelFace::RIGHT).end());
    vertices.insert(vertices.end(),
                    chunk->GetBorderMesh(VoxelFace::BACK).begin(),
                    chunk->GetBorderMesh(VoxelFace::BACK).end());
    vertices.insert(vertices.end(),
                    chunk->GetBorderMesh(VoxelFace::LEFT).begin(),
                    chunk->GetBorderMesh(VoxelFace::LEFT).end());

    if (vertices.empty())
        return;

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
    uint32_t faceCount = vertices.size() / 4;
    for (uint32_t i = 0; i < faceCount; ++i)
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
    metadata.ModelMatrix = chunk->GetModelMatrix();

    m_RenderMetadata.insert({chunk->GetPosition(), metadata});
}
}; // namespace Terrain
