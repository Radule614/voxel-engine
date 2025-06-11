#pragma once

#include "../../Physics/PhysicsEngine.hpp"

namespace VoxelEngine
{

class ColliderComponent
{
public:
    ColliderComponent(JPH::BodyID bodyId);
    ~ColliderComponent();

    JPH::BodyID GetBodyId() const;

private:
    JPH::BodyID m_BodyId;
};

}
