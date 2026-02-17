//
// Created by RadU on 2/5/2026.
//

#include "EcsLayer.hpp"

#include "Ecs.hpp"
#include "Components/ParentComponent.hpp"
#include "glm/gtc/quaternion.hpp"

namespace VoxelEngine
{

static void CalculateTransform(TransformComponent& transform, const TransformComponent& parentTransform);
static void UpdateTransformRecursive(entt::entity entity, const TransformComponent& parentTransform);

EcsLayer::EcsLayer(EngineState& state) : m_State(state)
{
}

void EcsLayer::OnAttach() { Layer::OnAttach(); }

void EcsLayer::OnUpdate(GLCore::Timestep ts)
{
    auto& registry = EntityComponentSystem::Instance().GetEntityRegistry();

    const auto rootView = registry.view<TransformComponent>(entt::exclude<ParentComponent>);

    for (const auto rootEntity: rootView)
        UpdateTransformRecursive(rootEntity, TransformComponent{});
}

static void CalculateTransform(TransformComponent& transform, const TransformComponent& parentTransform)
{
    if (transform.IsDirty)
    {
        transform.LocalMatrix = glm::translate(glm::mat4(1.0f), transform.LocalPosition) *
                                glm::mat4_cast(transform.LocalRotation) *
                                glm::scale(glm::mat4(1.0f), transform.LocalScale);

        transform.IsDirty = false;
    }

    transform.PreviousWorldPosition = glm::vec3(transform.WorldMatrix[3]);
    transform.WorldMatrix = parentTransform.WorldMatrix * transform.LocalMatrix;
    transform.WorldPosition = transform.WorldMatrix[3];
}

static void UpdateTransformRecursive(const entt::entity entity, const TransformComponent& parentTransform)
{
    static auto& registry = EntityComponentSystem::Instance().GetEntityRegistry();

    auto transform = registry.try_get<TransformComponent>(entity);
    if (transform == nullptr)
        return;

    CalculateTransform(*transform, parentTransform);

    const auto children = registry.try_get<ChildrenComponent>(entity);
    if (children == nullptr)
        return;

    for (const auto& child: children->Entities)
        UpdateTransformRecursive(child, *transform);
}

}
