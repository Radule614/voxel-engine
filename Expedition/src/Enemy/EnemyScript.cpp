#include "EnemyScript.hpp"

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
    auto& registry = context.Registry;
    auto& transform = registry.get<TransformComponent>(context.Entity);
    auto& character = registry.get<CharacterComponent>(context.Entity);

    // Find player (entity with CameraComponent + CharacterComponent)
    glm::vec3 playerPos = transform.WorldPosition;
    bool foundPlayer = false;

    for (const auto view = registry.view<TransformComponent, CharacterComponent, CameraComponent>();
         const auto playerEntity : view)
    {
        playerPos = registry.get<TransformComponent>(playerEntity).WorldPosition;
        foundPlayer = true;
        break;
    }

    if (!foundPlayer)
    {
        character.Controller->HandleInput(JPH::Vec3::sZero(), false, ts.GetSeconds());
        return;
    }

    glm::vec3 dir = playerPos - transform.WorldPosition;
    dir.y = 0.0f;
    const float distance = glm::length(dir);

    if (distance > m_ChaseRadius || distance < 0.001f)
    {
        character.Controller->HandleInput(JPH::Vec3::sZero(), false, ts.GetSeconds());
        return;
    }

    dir = glm::normalize(dir) * m_ChaseSpeed;
    character.Controller->HandleInput(JPH::Vec3(dir.x, dir.y, dir.z), false, ts.GetSeconds());
}

void EnemyScript::OnEvent(Event& event, ScriptContext context)
{
}

}
