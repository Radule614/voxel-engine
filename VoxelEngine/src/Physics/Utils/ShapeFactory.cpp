//
// Created by RadU on 09-Jun-25.
//

#include "ShapeFactory.hpp"
#include "Jolt/Physics/Collision/Shape/BoxShape.h"
#include "Jolt/Physics/Collision/Shape/CapsuleShape.h"
#include "Jolt/Physics/Collision/Shape/SphereShape.h"

using namespace JPH;

namespace VoxelEngine
{

ShapeFactory::ShapeFactory()
{
}

ShapeFactory::~ShapeFactory() = default;

ShapeRefC ShapeFactory::CreateSphereShape(const float_t radius) const
{
    const SphereShapeSettings shapeSettings(radius);
    shapeSettings.SetEmbedded();
    return shapeSettings.Create().Get();
}

ShapeRefC ShapeFactory::CreateBoxShape(const glm::vec3 halfSize) const
{
    const BoxShapeSettings shapeSettings(Vec3(halfSize.x, halfSize.y, halfSize.z));
    shapeSettings.SetEmbedded();
    return shapeSettings.Create().Get();
}

ShapeRefC ShapeFactory::CreateCapsuleShape(const float_t height, const float_t radius) const
{
    const CapsuleShapeSettings shapeSettings(height, radius);
    shapeSettings.SetEmbedded();
    return shapeSettings.Create().Get();
}

}
