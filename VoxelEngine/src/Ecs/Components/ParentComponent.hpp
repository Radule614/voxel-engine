//
// Created by RadU on 2/17/2026.
//

#pragma once

#include "ChildrenComponent.hpp"

namespace VoxelEngine
{

struct ParentComponent
{
    entt::entity Entity;

    explicit ParentComponent(const entt::entity entity) : Entity(entity)
    {
    }
};

}
