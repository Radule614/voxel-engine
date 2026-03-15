#pragma once

#include "Ecs/Components/ScriptComponent.hpp"
#include "Physics/PhysicsEngine.hpp"

#include "entt.hpp"
#include <vector>

namespace Expedition
{

struct PlayerScript : VoxelEngine::Script
{
    explicit PlayerScript();
    void OnUpdate(GLCore::Timestep ts, VoxelEngine::ScriptContext context) override;
    void OnEvent(GLCore::Event& event, VoxelEngine::ScriptContext context) override;
    void OnDetach(VoxelEngine::ScriptContext context) override;

private:
    float m_AttackDuration = 0.8f;
    float m_AttackTimer    = 0.0f;
    bool  m_Attacking      = false;
    float m_SavedSpeed     = 0.0f;

    struct BallEntry { entt::entity Entity; JPH::BodyID BodyId; float Age = 0.0f; };
    std::vector<BallEntry> m_Balls;
};

}
