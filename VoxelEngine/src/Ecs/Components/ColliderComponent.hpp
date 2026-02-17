#pragma once

#include "../../Physics/PhysicsEngine.hpp"
#include "../ComponentGui.hpp"

namespace VoxelEngine
{

struct ColliderComponent : ComponentGui
{
    JPH::BodyID BodyId;
    JPH::EShapeType ShapeType;
    JPH::EShapeSubType ShapeSubType;

    explicit ColliderComponent(JPH::BodyID bodyId, JPH::EShapeType type, JPH::EShapeSubType subType);
    ~ColliderComponent() override = default;

    std::string GetName() override;
    void DrawGui() override;
};

}
