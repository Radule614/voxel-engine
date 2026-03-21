#pragma once

#include "Ecs/Components/AnimationComponent.hpp"
#include "Ecs/Components/ChildrenComponent.hpp"
#include "Ecs/Components/ScriptComponent.hpp"

namespace Expedition
{

struct EnemyScript : VoxelEngine::Script
{
    explicit EnemyScript();
    void OnUpdate(GLCore::Timestep ts, VoxelEngine::ScriptContext context) override;
    void OnEvent(GLCore::Event& event, VoxelEngine::ScriptContext context) override;
    void OnDetach(VoxelEngine::ScriptContext context) override;

    enum class State { Idle, Chasing, Attacking, Dying };

private:
    static VoxelEngine::AnimationComponent* FindAnimComponent(
        entt::registry& registry, entt::entity entity);
    static bool IsClipFinished(VoxelEngine::AnimationComponent* anim, const char* name);
    static void ActivateClip(VoxelEngine::AnimationComponent* anim,
                             const char* name, bool loops);

    float m_ChaseSpeed = 4.0f;
    float m_StopRadius = 1.5f;
    float m_Gravity    = 9.8f * 3.0f;
    State m_State      = State::Idle;
};

}
