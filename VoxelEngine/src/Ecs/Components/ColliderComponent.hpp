#pragma once

#include "../../Physics/PhysicsEngine.hpp"

namespace VoxelEngine
{

struct ColliderComponent
{
    JPH::BodyID BodyId;

    ColliderComponent()
    {
    }

    explicit ColliderComponent(const JPH::BodyID bodyId) : BodyId(bodyId)
    {
    }
};

}
