#pragma once

#include <unordered_map>

#include "entt.hpp"
#include "GLCore.hpp"
#include "GLCoreUtils.hpp"
#include "World/World.hpp"
#include "../EngineState.hpp"
#include "../Physics/PhysicsEngine.hpp"
#include "../Renderer/Renderer.hpp"

namespace VoxelEngine
{

class VoxelLayer : public GLCore::Layer
{
public:
    explicit VoxelLayer(EngineState& state);
    ~VoxelLayer() override;

    void OnAttach() override;
    void OnDetach() override;
    void OnEvent(GLCore::Event& event) override;
    void OnUpdate(GLCore::Timestep ts) override;
    void OnImGuiRender() override;

    // Must be called before attaching
    void Init(WorldSettings&& settings);

private:
    struct UIState
    {
        int32_t ThreadCount = TerrainConfig::ThreadCount - 1;
        int32_t PolygonMode = TerrainConfig::PolygonMode == GL_FILL ? 0 : 1;
    };

    void PollChunkRenderQueue() const;
    void SetupRenderData(const std::shared_ptr<Chunk>& chunk) const;
    void ApplyState() const;
    void ResetWorld() const;
    void RemoveDistantChunks() const;

    void DeleteChunkRenderData(const Chunk& chunk) const;

    void CreateTerrainCollider() const;
    void OnColliderLocationChanged(glm::vec3 pos);
    void OptimizeColliders();

private:
    EngineState& m_EngineState;
    UIState m_UIState;
    std::unique_ptr<World> m_World;
    std::unordered_map<Position2D, ChunkRenderData>* m_RenderData;
    std::unordered_set<glm::i32vec3> m_ColliderPositions;
    float_t timeSinceLastColliderOptimization = 0.0f;

    JPH::ShapeRefC m_VoxelShape;
    entt::entity m_TerrainEntityId;
};

};
