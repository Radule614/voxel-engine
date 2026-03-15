//
// Created by RadU on 3/15/2026.
//

#include "Ecs.hpp"
#include "EcsUtils.hpp"

#include "Components/ParentComponent.hpp"
#include "Components/ChildrenComponent.hpp"

namespace VoxelEngine
{

void RemoveChildFromEntity(const entt::entity entity, const entt::entity child)
{
    auto& registry = EntityComponentSystem::Instance().GetEntityRegistry();

    registry.remove<ParentComponent>(child);

    ChildrenComponent* children = registry.try_get<ChildrenComponent>(entity);
    if (children == nullptr)
        return;

    auto& entities = children->Entities;

    entities.erase(std::ranges::remove(entities, entity).begin(), entities.end());
    if (entities.empty())
        registry.remove<ChildrenComponent>(entity);
}

void AddChildToEntity(const entt::entity entity, const entt::entity child)
{
    auto& registry = EntityComponentSystem::Instance().GetEntityRegistry();

    registry.emplace<ParentComponent>(child, entity);

    ChildrenComponent* children = registry.try_get<ChildrenComponent>(entity);
    if (children == nullptr)
        children = &registry.emplace<ChildrenComponent>(entity);

    children->Entities.emplace_back(child);
}

}
