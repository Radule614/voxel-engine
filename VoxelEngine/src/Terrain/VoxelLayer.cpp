#include "VoxelLayer.hpp"
#include "VoxelMeshBuilder.hpp"
#include <vector>
#include "../Assets/Vertex.hpp"
#include "World.hpp"
#include "../Ecs/Components/PlayerComponent.hpp"
#include "../Physics/Utils/BodyBuilder.hpp"
#include "../Physics/Utils/JoltUtils.hpp"

using namespace GLCore;
using namespace GLCore::Utils;
using namespace JPH;

namespace VoxelEngine
{

VoxelLayer::VoxelLayer(EngineState& state)
    : Layer("VoxelLayer"),
      m_EngineState(state),
      m_World(World(state.CameraController)),
      m_VoxelColliders({})
{
    m_VoxelShape = ShapeFactory().CreateBoxShape(glm::vec3(0.5f));

    auto& registry = EntityComponentSystem::Instance().GetEntityRegistry();
    m_TerrainEntityId = registry.create();
    registry.emplace<TerrainComponent>(m_TerrainEntityId);
    m_RenderData = &registry.get<TerrainComponent>(m_TerrainEntityId).RenderData;
}

VoxelLayer::~VoxelLayer() = default;

void VoxelLayer::OnAttach() { m_World.StartGeneration(); }

void VoxelLayer::OnDetach()
{
    m_World.StopGeneration();
    auto& renderDataMap = *m_RenderData;
    for (auto& it: renderDataMap)
    {
        ChunkRenderData& data = it.second;
        glDeleteBuffers(1, &data.VertexBuffer);
        glDeleteBuffers(1, &data.IndexBuffer);
        glDeleteVertexArrays(1, &data.VertexArray);
        data.Indices.clear();
    }
    renderDataMap.clear();
}

void VoxelLayer::OnEvent(Event& event)
{
    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<WindowCloseEvent>(
        [&](WindowCloseEvent& e) {
            m_World.StopGeneration();
            return false;
        });
    dispatcher.Dispatch<StatePauseEvent>(
        [&](StatePauseEvent& e) {
            m_World.StopGeneration();
            const UIState state;
            m_UIState = state;
            return false;
        });
    dispatcher.Dispatch<StateUnpauseEvent>(
        [&](StateUnpauseEvent& e) {
            m_World.StartGeneration();
            return false;
        });
    dispatcher.Dispatch<ColliderLocationChangedEvent>(
        [&](const ColliderLocationChangedEvent& e) {
            OnColliderLocationChanged(e.GetLocation());
            return true;
        });
}

void VoxelLayer::OnUpdate(const Timestep ts)
{
    CheckChunkRenderQueue();
    timeSinceLastColliderOptimization += ts;
    if (timeSinceLastColliderOptimization >= 3.0f)
    {
        timeSinceLastColliderOptimization = 0.0f;
        OptimizeColliders();
    }
}

void VoxelLayer::OnImGuiRender()
{
    if (!m_EngineState.MenuActive)
        return;

    constexpr ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
                                             ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar |
                                             ImGuiWindowFlags_NoMove;
    const auto& io = ImGui::GetIO();
    ImGui::SetNextWindowSize(ImVec2(400.0, 600.0));
    ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - 400.0, 0));

    ImGui::Begin("Terrain Settings", nullptr, windowFlags);
    ImGui::Text("Terrain Settings");

    const char* polygonModes[] = {"Fill", "Line"};
    ImGui::Combo("Polygon Mode", &m_UIState.PolygonMode, polygonModes, IM_ARRAYSIZE(polygonModes));
    const char* threadCounts[] = {"1", "2", "3", "4"};
    ImGui::Combo("Thread Count", &m_UIState.ThreadCount, threadCounts, IM_ARRAYSIZE(threadCounts));

    if (ImGui::Button("Apply"))
        ApplyState();

    ImGui::End();
}

void VoxelLayer::ApplyState() const
{
    TerrainConfig::PolygonMode = m_UIState.PolygonMode == 0 ? GL_FILL : GL_LINE;
    TerrainConfig::ThreadCount = m_UIState.ThreadCount + 1;
    glPolygonMode(GL_FRONT_AND_BACK, TerrainConfig::PolygonMode);
}

void VoxelLayer::OnColliderLocationChanged(const glm::vec3 pos)
{
    auto& chunkMap = m_World.GetChunkMap();
    const auto chunkIterator = m_World.GetChunkMap().find(m_World.WorldToChunkSpace(pos));
    if (chunkIterator == chunkMap.end())
        return;
    constexpr int32_t r = 2;
    for (int32_t x = -r; x <= r; ++x)
    {
        for (int32_t z = -r; z <= r; ++z)
        {
            for (int32_t y = -r; y <= r; ++y)
            {
                auto p = glm::i32vec3(glm::round(pos)) + glm::i32vec3(x, y, z);
                auto [chunkPosition, voxelPosition] = m_World.GetPositionInWorld(p);
                auto it = chunkMap.find(chunkPosition);
                if (it == chunkMap.end() || !InRange(p.y, 0, CHUNK_HEIGHT - 1))
                    continue;
                Voxel& v = it->second->GetVoxelGrid()[voxelPosition.GetX()][voxelPosition.GetZ()][voxelPosition.y];
                if (v.GetVoxelType() == AIR || m_VoxelColliders.contains(p))
                    continue;

                const BodyID bodyId = BodyBuilder().SetShape(m_VoxelShape).SetPosition(p).BuildAndAdd();
                m_VoxelColliders.insert(std::make_pair(p, ColliderComponent(bodyId)));
            }
        }
    }
}

void VoxelLayer::OptimizeColliders()
{
    LOG_INFO("Body count before optimization: {0}", PhysicsEngine::Instance().GetSystem().GetNumBodies());
    PhysicsSystem& physicsSystem = PhysicsEngine::Instance().GetSystem();
    BodyInterface& bodyInterface = physicsSystem.GetBodyInterface();
    std::unordered_map collidersToRemove(m_VoxelColliders);
    BodyIDVector bodies;
    physicsSystem.GetBodies(bodies);

    const auto& playerView = EntityComponentSystem::Instance().GetEntityRegistry().view<PlayerComponent>();

    for (auto& [voxelPosition, collider]: m_VoxelColliders)
    {
        for (auto bodyId: bodies)
        {
            if (const ObjectLayer layer = bodyInterface.GetObjectLayer(bodyId); layer != Layers::MOVING)
                continue;
            glm::vec3 position = JoltUtils::JoltToGlmVec3(bodyInterface.GetPosition(bodyId));
            const float_t distance = glm::distance(position, static_cast<glm::vec3>(voxelPosition));
            if (distance < 3 && collidersToRemove.contains(voxelPosition))
                collidersToRemove.erase(voxelPosition);
        }

        for (const auto& entity: playerView)
        {
            const auto& character = *playerView.get<PlayerComponent>(entity).Character;
            glm::vec3 position = JoltUtils::JoltToGlmVec3(character.GetPosition());
            const float_t distance = glm::distance(position, static_cast<glm::vec3>(voxelPosition));
            if (distance < 3 && collidersToRemove.contains(voxelPosition))
                collidersToRemove.erase(voxelPosition);
        }
    }

    for (auto& [pos, collider]: collidersToRemove)
    {
        m_VoxelColliders.erase(m_VoxelColliders.find(pos));
        bodyInterface.RemoveBody(collider.BodyId);
        bodyInterface.DestroyBody(collider.BodyId);
    }
    LOG_INFO("Body count after optimization: {0}", PhysicsEngine::Instance().GetSystem().GetNumBodies());
}

void VoxelLayer::CheckChunkRenderQueue()
{
    auto& worldLock = m_World.GetLock();
    auto& chunks = m_World.GetChangedChunks();
    if (chunks.empty() || !worldLock.try_lock())
        return;
    auto it = chunks.begin();
    while (it != chunks.end())
    {
        std::shared_ptr<Chunk> chunk = *it;
        auto& chunkLock = chunk->GetLock();
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

void VoxelLayer::SetupRenderData(const std::shared_ptr<Chunk>& chunk) const
{
    ChunkRenderData data = {};
    auto& renderDataMap = *m_RenderData;
    if (const auto renderData = renderDataMap.find(chunk->GetPosition()); renderData != renderDataMap.end())
    {
        ChunkRenderData& m = renderData->second;
        glDeleteVertexArrays(1, &m.VertexArray);
        glDeleteBuffers(1, &m.VertexBuffer);
        glDeleteBuffers(1, &m.IndexBuffer);
        m.Indices.clear();
        renderDataMap.erase(chunk->GetPosition());
    }

    std::vector<VoxelVertex> vertices = {};
    vertices.insert(vertices.end(), chunk->GetMesh().begin(), chunk->GetMesh().end());
    vertices.insert(vertices.end(),
                    chunk->GetBorderMesh(FRONT).begin(),
                    chunk->GetBorderMesh(FRONT).end());
    vertices.insert(vertices.end(),
                    chunk->GetBorderMesh(RIGHT).begin(),
                    chunk->GetBorderMesh(RIGHT).end());
    vertices.insert(vertices.end(),
                    chunk->GetBorderMesh(BACK).begin(),
                    chunk->GetBorderMesh(BACK).end());
    vertices.insert(vertices.end(),
                    chunk->GetBorderMesh(LEFT).begin(),
                    chunk->GetBorderMesh(LEFT).end());

    if (vertices.empty())
        return;

    glCreateVertexArrays(1, &data.VertexArray);
    glBindVertexArray(data.VertexArray);

    glCreateBuffers(1, &data.VertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, data.VertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VoxelVertex), &vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VoxelVertex), nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(VoxelVertex),
                          reinterpret_cast<void*>(offsetof(VoxelVertex, VoxelVertex::Normal)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(VoxelVertex),
                          reinterpret_cast<void*>(offsetof(VoxelVertex, VoxelVertex::TexCoords)));

    std::vector<uint32_t> indices = {};
    const uint32_t faceCount = vertices.size() / 4;
    for (uint32_t i = 0; i < faceCount; ++i)
    {
        indices.push_back(i * 4 + 0);
        indices.push_back(i * 4 + 1);
        indices.push_back(i * 4 + 2);
        indices.push_back(i * 4 + 2);
        indices.push_back(i * 4 + 3);
        indices.push_back(i * 4 + 0);
    }
    glCreateBuffers(1, &data.IndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.IndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), &indices[0], GL_STATIC_DRAW);

    glBindVertexArray(0);

    data.Indices = indices;
    data.ModelMatrix = chunk->GetModelMatrix();

    renderDataMap.insert({chunk->GetPosition(), data});
}

};
