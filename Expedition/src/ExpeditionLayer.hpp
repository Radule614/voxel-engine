#pragma once

#include <GLCore.hpp>
#include "EngineState.hpp"
#include "entt.hpp"

namespace VoxelEngine { class World; }

namespace Expedition
{

class ExpeditionLayer : public GLCore::Layer
{
public:
    ExpeditionLayer(VoxelEngine::EngineState& state, VoxelEngine::World& world);
    void OnAttach() override;
    void OnUpdate(GLCore::Timestep ts) override;
    void OnImGuiRender() override;

private:
    void      SpawnPlayer();
    void      SpawnEnemy(glm::vec3 position);
    void      SpawnHealthBar(entt::entity enemyParent);
    glm::vec3 RandomSpawnNearPlayer(float minR, float maxR) const;

    VoxelEngine::EngineState& m_State;
    VoxelEngine::World&       m_World;
};

}
