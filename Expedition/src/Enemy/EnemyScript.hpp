#pragma once

#include <vector>
#include "Ecs/Components/AnimationComponent.hpp"
#include "Ecs/Components/ChildrenComponent.hpp"
#include "Ecs/Components/ScriptComponent.hpp"
#include "Navigation/Pathfinder.hpp"

namespace VoxelEngine { class World; }

namespace Expedition
{

struct EnemyScript : VoxelEngine::Script
{
    explicit EnemyScript(VoxelEngine::World& world);
    void OnUpdate(GLCore::Timestep ts, VoxelEngine::ScriptContext context) override;
    void OnEvent(GLCore::Event& event, VoxelEngine::ScriptContext context) override;
    void OnDetach(VoxelEngine::ScriptContext context) override;

    enum class State { Idle, Chasing, Attacking, Dying, Jumping };

private:
    static VoxelEngine::AnimationComponent* FindAnimComponent(
        entt::registry& registry, entt::entity entity);
    static bool IsClipFinished(VoxelEngine::AnimationComponent* anim, const char* name);
    static void ActivateClip(VoxelEngine::AnimationComponent* anim,
                             const char* name, bool loops);

    float     m_ChaseSpeed    = 6.0f;
    float     m_StopRadius    = 1.5f;
    float     m_Gravity       = 9.8f * 3.0f;
    float     m_JumpSpeed     = 15.0f;
    float     m_BlockedTime   = 0.0f;
    float     m_BlockedThresh = 0.3f;  // seconds stuck before jumping
    bool      m_WasAirborne          = false; // true once airborne during a jump
    bool      m_RepathedWhileBlocked = false; // true after first repath attempt when stuck
    glm::vec3 m_LastPosition  = glm::vec3(0.0f);
    bool      m_HasLastPos    = false;
    State     m_State         = State::Idle;

    VoxelEngine::World& m_World;

    // A* pathfinding
    std::vector<Waypoint> m_Path;
    int   m_PathIndex   = 0;
    float m_PathTimer   = 0.0f;
    float m_RethinkTime = 0.5f;
    float m_WaypointDist = 1.5f;
};

}
