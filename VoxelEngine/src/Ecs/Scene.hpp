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
        auto& registry = EntityComponentSystem::Instance().GetEntityRegistry();
        auto& view = registry.get<ChildrenComponent>(Entity).Entities;

        view.erase(std::ranges::remove(view, child).begin(), view.end());
    }

    void AddChild(const entt::entity entity) const
    {
        auto& registry = EntityComponentSystem::Instance().GetEntityRegistry();

        auto children = registry.try_get<ChildrenComponent>(Entity);
        if (children == nullptr)
            children = &registry.emplace<ChildrenComponent>(Entity);

        children->Entities.emplace_back(entity);
    }
};

struct MetadatComponent
{
    std::string Name;
};

}
