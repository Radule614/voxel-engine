#include "EnemyScript.hpp"
#include "EnemyComponent.hpp"

#include "Ecs/Components/CameraComponent.hpp"
#include "Ecs/Components/CharacterComponent.hpp"
#include "Ecs/Components/TransformComponent.hpp"

using namespace VoxelEngine;
using namespace GLCore;

namespace Expedition
{

EnemyScript::EnemyScript() : Script("Enemy Script")
{
}

void EnemyScript::OnUpdate(const Timestep ts, ScriptContext context)
{
    auto& registry  = context.Registry;
    auto& transform = registry.get<TransformComponent>(context.Entity);
    auto& enemy     = registry.get<EnemyComponent>(context.Entity);

    JPH::Character& character = *enemy.Character;

    // Refresh ground state after the physics step that just ran
    character.PostSimulation(0.05f);

    // Accumulate gravity when airborne; reset on landing
    if (character.GetGroundState() == JPH::Character::EGroundState::OnGround)
        enemy.VerticalVelocity = 0.0f;
    else
        enemy.VerticalVelocity -= m_Gravity * ts.GetSeconds();

    // Find the player entity
    glm::vec3 playerPos = transform.WorldPosition;
    bool foundPlayer    = false;

    for (const auto view = registry.view<TransformComponent, CharacterComponent, CameraComponent>();
         const auto playerEntity : view)
    {
        playerPos    = registry.get<TransformComponent>(playerEntity).WorldPosition;
        foundPlayer  = true;
        break;
    }

    // Chase logic
    JPH::Vec3 horizontal = JPH::Vec3::sZero();
    State     state      = State::Idle;
    glm::vec3 faceDir    = glm::vec3(0.0f, 0.0f, 1.0f);
    if (foundPlayer)
    {
        glm::vec3 dir    = playerPos - transform.WorldPosition;
        dir.y            = 0.0f;
        const float dist = glm::length(dir);

        if (dist > 0.001f)
            faceDir = glm::normalize(dir);

        if (dist <= m_StopRadius)
        {
            state = State::Attacking;
        }
        else if (dist <= m_ChaseRadius)
        {
            horizontal = JPH::Vec3(faceDir.x * m_ChaseSpeed, 0.0f, faceDir.z * m_ChaseSpeed);
            state      = State::Chasing;
        }
    }

    // Drive animation and rotation on the child model entity
    if (const auto* children = registry.try_get<ChildrenComponent>(context.Entity))
    {
        for (const auto child : children->Entities)
        {
            auto* childTransform = registry.try_get<TransformComponent>(child);
            if (childTransform)
            {
                childTransform->LocalRotation = glm::angleAxis(atan2(faceDir.x, faceDir.z), glm::vec3(0, 1, 0));
                childTransform->IsDirty = true;
            }

            if (auto* anim = registry.try_get<AnimationComponent>(child))
            {
                const bool stateChanged = state != m_LastState;
                for (auto& clip : anim->Animations)
                {
                    const bool shouldBeActive =
                        (clip.Name == "idle"   && state == State::Idle)     ||
                        (clip.Name == "run"    && state == State::Chasing)  ||
                        (clip.Name == "attack" && state == State::Attacking);

                    if (stateChanged && shouldBeActive)
                        clip.Time = 0.0f;

                    clip.IsActive = shouldBeActive;
                }
            }

            if (childTransform)
                break;
        }
    }

    m_LastState = state;

    // Keep the body awake and apply velocity
    character.Activate();
    character.SetLinearVelocity(JPH::Vec3(horizontal.GetX(), enemy.VerticalVelocity, horizontal.GetZ()));
}

void EnemyScript::OnEvent(Event& event, ScriptContext context)
{
}

void EnemyScript::OnDetach(ScriptContext context)
{
    auto& enemy = context.Registry.get<EnemyComponent>(context.Entity);
    if (enemy.Character)
        enemy.Character->RemoveFromPhysicsSystem();
}

}
