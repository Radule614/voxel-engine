#include "HealthBarScript.hpp"

#include <glm/gtc/quaternion.hpp>
#include "Health/HealthComponent.hpp"
#include "Ecs/Ecs.hpp"
#include "Ecs/Components/CameraComponent.hpp"
#include "Ecs/Components/TransformComponent.hpp"

using namespace VoxelEngine;
using namespace GLCore;

namespace Expedition
{

HealthBarScript::HealthBarScript(entt::entity rootEntity, entt::entity fillEntity, float barWidth)
    : Script("Health Bar Script"),
      m_RootEntity(rootEntity),
      m_FillEntity(fillEntity),
      m_BarWidth(barWidth)
{
}

void HealthBarScript::OnUpdate(const Timestep ts, ScriptContext context)
{
    auto& registry = context.Registry;

    auto* health = registry.try_get<HealthComponent>(context.Entity);
    if (!health)
        return;

    const float pct = glm::clamp(health->CurrentHealth / health->MaxHealth, 0.0f, 1.0f);

    // Find camera world position for billboard rotation
    glm::vec3 camPos = registry.get<TransformComponent>(context.Entity).WorldPosition;
    for (const auto view = registry.view<TransformComponent, CameraComponent>();
         const auto e : view)
    {
        camPos = registry.get<TransformComponent>(e).WorldPosition;
        break;
    }

    // Billboard: rotate root to face camera around Y axis
    auto* rootT = registry.try_get<TransformComponent>(m_RootEntity);
    if (rootT)
    {
        const glm::vec3 enemyPos = registry.get<TransformComponent>(context.Entity).WorldPosition;
        glm::vec3 toCamera = camPos - enemyPos;
        toCamera.y = 0.0f;
        if (glm::length(toCamera) > 0.001f)
        {
            toCamera = glm::normalize(toCamera);
            const float angle = atan2(toCamera.x, toCamera.z);
            rootT->LocalRotation = glm::angleAxis(angle, glm::vec3(0.0f, 1.0f, 0.0f));
        }
        rootT->IsDirty = true;
    }

    // Update fill: scale X by health %, left-align
    auto* fillT = registry.try_get<TransformComponent>(m_FillEntity);
    if (fillT)
    {
        // Fill is in bg's local space (bg already has barWidth scale),
        // so scale and position are normalized 0-1 values
        fillT->LocalScale.x    = pct;
        fillT->LocalPosition.x = -(1.0f - pct) / 2.0f;
        fillT->IsDirty         = true;
    }
}

void HealthBarScript::OnEvent(Event& event, ScriptContext context)
{
}

void HealthBarScript::OnDetach(ScriptContext context)
{
    EntityComponentSystem::Instance().DestroyEntityRecursive(m_RootEntity);
}

}
