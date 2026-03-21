#include "EnemyScript.hpp"
#include "EnemyComponent.hpp"

#include "Ecs/Components/CameraComponent.hpp"
#include "Ecs/Components/CharacterComponent.hpp"
#include "Ecs/Components/TransformComponent.hpp"
#include "Health/HealthComponent.hpp"

using namespace VoxelEngine;
using namespace GLCore;

namespace Expedition
{

// ── State → animation mapping ───────────────────────────────────────
struct StateInfo
{
    const char* clipName;
    bool        loops;
    bool        freezeMove;
};

static constexpr StateInfo kStateInfo[] = {
    /* Idle      */ { "idle",   true,  false },
    /* Chasing   */ { "run",    true,  false },
    /* Attacking */ { "attack", false, true  },
    /* Dying     */ { "death",  false, true  },
};

// ── Helpers ─────────────────────────────────────────────────────────
AnimationComponent* EnemyScript::FindAnimComponent(
    entt::registry& registry, entt::entity entity)
{
    const auto* children = registry.try_get<ChildrenComponent>(entity);
    if (!children)
        return nullptr;

    for (const auto child : children->Entities)
    {
        if (auto* anim = registry.try_get<AnimationComponent>(child))
            return anim;
        if (registry.try_get<TransformComponent>(child))
            break;
    }
    return nullptr;
}

bool EnemyScript::IsClipFinished(AnimationComponent* anim, const char* name)
{
    if (!anim)
        return false;
    for (const auto& clip : anim->Animations)
    {
        if (clip.Name == name)
            return !clip.ShouldRepeat && !clip.IsActive;
    }
    return false;
}

void EnemyScript::ActivateClip(AnimationComponent* anim,
                               const char* name, bool loops)
{
    if (!anim)
        return;
    for (auto& clip : anim->Animations)
    {
        if (clip.Name == name)
        {
            clip.Time         = 0.0f;
            clip.ShouldRepeat = loops;
            clip.IsActive     = true;
        }
        else
        {
            clip.IsActive = false;
        }
    }
}

// ── Constructor ─────────────────────────────────────────────────────
EnemyScript::EnemyScript() : Script("Enemy Script")
{
}

// ── OnUpdate ────────────────────────────────────────────────────────
void EnemyScript::OnUpdate(const Timestep ts, ScriptContext context)
{
    auto& registry = context.Registry;
    auto& enemy    = registry.get<EnemyComponent>(context.Entity);
    auto& health   = registry.get<HealthComponent>(context.Entity);
    auto* anim     = FindAnimComponent(registry, context.Entity);

    // ── Dying: wait for animation, then signal removal ──────────
    if (m_State == State::Dying)
    {
        if (IsClipFinished(anim, kStateInfo[static_cast<int>(State::Dying)].clipName))
            enemy.ReadyToRemove = true;
        return;
    }

    // ── Just died: enter Dying state ────────────────────────────
    if (health.IsDead())
    {
        m_State = State::Dying;
        enemy.IsDying = true;

        if (enemy.Character)
        {
            enemy.Character->SetLinearVelocity(JPH::Vec3::sZero());
            enemy.Character->RemoveFromPhysicsSystem();
            enemy.Character = nullptr;
        }

        ActivateClip(anim,
                     kStateInfo[static_cast<int>(State::Dying)].clipName,
                     kStateInfo[static_cast<int>(State::Dying)].loops);
        return;
    }

    // ── Physics / gravity ───────────────────────────────────────
    auto& transform = registry.get<TransformComponent>(context.Entity);
    JPH::Character& character = *enemy.Character;

    character.PostSimulation(0.05f);

    if (character.GetGroundState() == JPH::Character::EGroundState::OnGround)
        enemy.VerticalVelocity = 0.0f;
    else
        enemy.VerticalVelocity -= m_Gravity * ts.GetSeconds();

    // ── Find player ─────────────────────────────────────────────
    glm::vec3 playerPos = transform.WorldPosition;
    bool foundPlayer    = false;

    for (const auto view = registry.view<TransformComponent, CharacterComponent, CameraComponent>();
         const auto playerEntity : view)
    {
        playerPos   = registry.get<TransformComponent>(playerEntity).WorldPosition;
        foundPlayer = true;
        break;
    }

    // ── Compute desired state from distance ─────────────────────
    JPH::Vec3 horizontal = JPH::Vec3::sZero();
    State     desiredState = State::Idle;
    glm::vec3 faceDir      = glm::vec3(0.0f, 0.0f, 1.0f);

    if (foundPlayer)
    {
        glm::vec3 dir = playerPos - transform.WorldPosition;
        dir.y         = 0.0f;
        const float dist = glm::length(dir);

        if (dist > 0.001f)
            faceDir = glm::normalize(dir);

        if (dist <= m_StopRadius)
            desiredState = State::Attacking;
        else
        {
            horizontal   = JPH::Vec3(faceDir.x * m_ChaseSpeed, 0.0f, faceDir.z * m_ChaseSpeed);
            desiredState = State::Chasing;
        }
    }

    // ── State transitions ───────────────────────────────────────
    const State prevState = m_State;

    if (m_State == State::Attacking)
    {
        // Lock until attack clip finishes
        if (IsClipFinished(anim, kStateInfo[static_cast<int>(State::Attacking)].clipName))
            m_State = desiredState;
        else
            horizontal = JPH::Vec3::sZero();
    }
    else
    {
        m_State = desiredState;
    }

    // ── Enter-state actions ─────────────────────────────────────
    if (m_State != prevState)
    {
        const auto& info = kStateInfo[static_cast<int>(m_State)];
        ActivateClip(anim, info.clipName, info.loops);

        // Deal damage on entering Attacking
        if (m_State == State::Attacking)
        {
            for (const auto view = registry.view<HealthComponent, CharacterComponent, CameraComponent>();
                 const auto playerEntity : view)
            {
                registry.get<HealthComponent>(playerEntity).TakeDamage(10.0f);
                break;
            }
        }
    }

    if (kStateInfo[static_cast<int>(m_State)].freezeMove)
        horizontal = JPH::Vec3::sZero();

    // ── Rotation ────────────────────────────────────────────────
    if (const auto* children = registry.try_get<ChildrenComponent>(context.Entity))
    {
        for (const auto child : children->Entities)
        {
            auto* childTransform = registry.try_get<TransformComponent>(child);
            if (childTransform)
            {
                childTransform->LocalRotation = glm::angleAxis(
                    atan2(faceDir.x, faceDir.z), glm::vec3(0, 1, 0));
                childTransform->IsDirty = true;
                break;
            }
        }
    }

    // ── Apply velocity ──────────────────────────────────────────
    character.Activate();
    character.SetLinearVelocity(
        JPH::Vec3(horizontal.GetX(), enemy.VerticalVelocity, horizontal.GetZ()));
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
