#pragma once

#include <GLCore.hpp>
#include "EngineState.hpp"

namespace Expedition
{

class WorldLayer : public GLCore::Layer
{
public:
    explicit WorldLayer(VoxelEngine::EngineState& state);
    void OnAttach() override;
    void OnUpdate(GLCore::Timestep ts) override;

private:
    void SpawnPlayer();
    void SpawnEnemy(glm::vec3 position);
    void SpawnHealthBar(entt::entity enemyParent);

    VoxelEngine::EngineState& m_State;
};

}
