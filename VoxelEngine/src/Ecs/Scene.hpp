//
// Created by RadU on 2/1/2026.
//

#pragma once

#include "entt.hpp"

namespace VoxelEngine
{

struct ChildrenComponent
{
    std::vector<entt::entity> Entities{};
};

struct ParentComponent
{
    entt::entity Entity;

    explicit ParentComponent(const entt::entity entity) : Entity(entity)
    {
    }

    void RemoveChild(const entt::entity child) const
    {
        static auto& registry = EntityComponentSystem::Instance().GetEntityRegistry();

        ChildrenComponent* children = registry.try_get<ChildrenComponent>(Entity);
        if (children == nullptr)
            return;

        auto& entities = children->Entities;

        entities.erase(std::ranges::remove(entities, child).begin(), entities.end());
    }

    void AddChild(const entt::entity entity) const
    {
        static auto& registry = EntityComponentSystem::Instance().GetEntityRegistry();

        ChildrenComponent* children = registry.try_get<ChildrenComponent>(Entity);
        if (children == nullptr)
            children = &registry.emplace<ChildrenComponent>(Entity);

        children->Entities.emplace_back(entity);
    }
};

}
