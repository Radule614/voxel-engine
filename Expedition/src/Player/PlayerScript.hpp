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
    struct BallEntry { entt::entity Entity; JPH::BodyID BodyId; };
    std::vector<BallEntry> m_Balls;
};

}
