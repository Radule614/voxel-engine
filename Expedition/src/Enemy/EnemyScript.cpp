#include "EnemyScript.hpp"
#include "EnemyComponent.hpp"

#include <cmath>
#include "Ecs/Components/CameraComponent.hpp"
#include "Ecs/Components/CharacterComponent.hpp"
#include "Ecs/Components/TransformComponent.hpp"
#include "Health/HealthComponent.hpp"
#include "GLCore/Core/Log.hpp"

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
    /* Jumping   */ { "jump",   false, false },
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
EnemyScript::EnemyScript(VoxelEngine::World& world)
    : Script("Enemy Script"), m_World(world)
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

    const bool isGrounded = character.GetGroundState() == JPH::Character::EGroundState::OnGround;

    if (isGrounded)
    {
        // Don't zero velocity while jumping — the impulse needs to persist
        if (m_State != State::Jumping)
            enemy.VerticalVelocity = 0.0f;
    }
    else
    {
        enemy.VerticalVelocity -= m_Gravity * ts.GetSeconds();
    }

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
    JPH::Vec3 horizontal    = JPH::Vec3::sZero();
    State     desiredState = State::Idle;
    glm::vec3 faceDir      = glm::vec3(0.0f, 0.0f, 1.0f);
    bool      proactiveJump = false;

    if (foundPlayer)
    {
        glm::vec3 dirToPlayer = playerPos - transform.WorldPosition;
        dirToPlayer.y         = 0.0f;
        const float distToPlayer = glm::length(dirToPlayer);

        if (distToPlayer > 0.001f)
            faceDir = glm::normalize(dirToPlayer);

        if (distToPlayer <= m_StopRadius)
        {
            desiredState = State::Attacking;
            m_Path.clear();
        }
        else
        {
            desiredState = State::Chasing;

            // Recalculate path periodically
            m_PathTimer += ts.GetSeconds();
            if (m_Path.empty() || m_PathTimer >= m_RethinkTime)
            {
                m_Path = Pathfinder(m_World).FindPath(transform.WorldPosition, playerPos);
                m_PathIndex = 0;
                m_PathTimer = 0.0f;

                int jumpCount = 0;
                float minY = 9999.f, maxY = -9999.f;
                for (const auto& wp : m_Path)
                {
                    if (wp.needsJump) ++jumpCount;
                    if (wp.position.y < minY) minY = wp.position.y;
                    if (wp.position.y > maxY) maxY = wp.position.y;
                }
                LOG_INFO("A* path: {} wps, {} jumps, Y range [{:.0f}, {:.0f}], enemy Y={:.0f}, player Y={:.0f}",
                         m_Path.size(), jumpCount, minY, maxY,
                         transform.WorldPosition.y, playerPos.y);
            }

            // Follow path waypoints
            if (!m_Path.empty() && m_PathIndex < static_cast<int>(m_Path.size()))
            {
                glm::vec3 toWaypoint = m_Path[m_PathIndex].position - transform.WorldPosition;
                toWaypoint.y = 0.0f;

                if (glm::length(toWaypoint) < m_WaypointDist)
                {
                    ++m_PathIndex;
                    if (m_PathIndex < static_cast<int>(m_Path.size()))
                    {
                        // Check if next waypoint requires a jump
                        if (m_Path[m_PathIndex].needsJump && isGrounded)
                        {
                            proactiveJump = true;
                            LOG_INFO("Proactive jump triggered at waypoint {}", m_PathIndex);
                        }

                        toWaypoint = m_Path[m_PathIndex].position - transform.WorldPosition;
                        toWaypoint.y = 0.0f;
                    }
                }

                if (m_PathIndex < static_cast<int>(m_Path.size()))
                {
                    const float len = glm::length(toWaypoint);
                    if (len > 0.001f)
                        faceDir = glm::normalize(toWaypoint);
                }
            }
            // else: no path → faceDir stays as direct-to-player (fallback)

            horizontal = JPH::Vec3(faceDir.x * m_ChaseSpeed, 0.0f, faceDir.z * m_ChaseSpeed);
        }
    }

    // ── Blocked detection ───────────────────────────────────────
    if (m_HasLastPos && desiredState == State::Chasing && isGrounded)
    {
        const glm::vec3 delta = transform.WorldPosition - m_LastPosition;
        const float hDist = glm::length(glm::vec2(delta.x, delta.z));
        if (hDist < m_ChaseSpeed * ts.GetSeconds() * 0.3f)
            m_BlockedTime += ts.GetSeconds();
        else
            m_BlockedTime = 0.0f;
    }
    else
    {
        m_BlockedTime = 0.0f;
    }

    // When stuck: first repath, if still stuck after second threshold → jump
    if (m_BlockedTime >= m_BlockedThresh && desiredState == State::Chasing)
    {
        if (!m_RepathedWhileBlocked)
        {
            // First attempt: force repath
            m_Path = Pathfinder(m_World).FindPath(transform.WorldPosition, playerPos);
            m_PathIndex = 0;
            m_PathTimer = 0.0f;
            m_BlockedTime = 0.0f;
            m_RepathedWhileBlocked = true;

            if (!m_Path.empty() && m_PathIndex < static_cast<int>(m_Path.size()))
            {
                glm::vec3 toWp = m_Path[m_PathIndex].position - transform.WorldPosition;
                toWp.y = 0.0f;
                if (glm::length(toWp) > 0.001f)
                    faceDir = glm::normalize(toWp);
            }
            horizontal = JPH::Vec3(faceDir.x * m_ChaseSpeed, 0.0f, faceDir.z * m_ChaseSpeed);
        }
        // else: still stuck after repath → will jump via state transition below
    }
    else if (m_BlockedTime < 0.01f)
    {
        m_RepathedWhileBlocked = false;
    }

    m_LastPosition = transform.WorldPosition;
    m_HasLastPos   = true;

    // ── Jump cooldown tick ─────────────────────────────────────
    if (m_State != State::Jumping)
        m_JumpCooldown += ts.GetSeconds();

    // ── State transitions ───────────────────────────────────────
    const State prevState = m_State;

    if (m_State == State::Jumping)
    {
        m_JumpTimer += ts.GetSeconds();

        if (!isGrounded)
            m_WasAirborne = true;

        // Exit: landed after being airborne, OR timed out
        if ((isGrounded && m_WasAirborne) || m_JumpTimer >= m_JumpTimeout)
        {
            m_State = desiredState;
            m_WasAirborne = false;
            m_JumpTimer   = 0.0f;
            m_JumpCooldown = 0.0f;
        }
    }
    else if (m_State == State::Attacking)
    {
        if (IsClipFinished(anim, kStateInfo[static_cast<int>(State::Attacking)].clipName))
            m_State = desiredState;
        else
            horizontal = JPH::Vec3::sZero();
    }
    else if (proactiveJump && m_JumpCooldown >= 1.0f)
    {
        m_State = State::Jumping;
    }
    else if (m_BlockedTime >= m_BlockedThresh && m_RepathedWhileBlocked
             && isGrounded && m_JumpCooldown >= 1.0f)
    {
        m_State = State::Jumping;
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

        if (m_State == State::Jumping)
        {
            // Scale jump velocity to height needed (min 8.0 for a reliable hop)
            float heightNeeded = 2.0f; // default
            if (!m_Path.empty() && m_PathIndex < static_cast<int>(m_Path.size()))
                heightNeeded = std::max(1.0f,
                    m_Path[m_PathIndex].position.y - transform.WorldPosition.y);
            // v = sqrt(2 * g * (h + margin))
            const float jumpV = std::sqrt(2.0f * m_Gravity * (heightNeeded + 1.0f));
            enemy.VerticalVelocity = std::min(jumpV, m_JumpSpeed);

            m_BlockedTime  = 0.0f;
            m_JumpTimer    = 0.0f;
            m_JumpCooldown = 0.0f;
        }

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
