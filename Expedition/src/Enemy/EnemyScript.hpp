#pragma once

#include "Ecs/Components/ScriptComponent.hpp"

namespace Expedition
{

struct EnemyScript : VoxelEngine::Script
{
    explicit EnemyScript();
    void OnUpdate(GLCore::Timestep ts, VoxelEngine::ScriptContext context) override;
    void OnEvent(GLCore::Event& event, VoxelEngine::ScriptContext context) override;

private:
    float m_ChaseSpeed = 4.0f;
    float m_ChaseRadius = 30.0f;
};

}
