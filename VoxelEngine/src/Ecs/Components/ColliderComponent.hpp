#pragma once

#include "../../Physics/PhysicsEngine.hpp"

namespace VoxelEngine
{

struct ColliderComponent
{
    ColliderComponent()
    {
    }

    ColliderComponent(const JPH::BodyID bodyId) : BodyId(bodyId)
    {
    }

    JPH::BodyID BodyId;
};

}
