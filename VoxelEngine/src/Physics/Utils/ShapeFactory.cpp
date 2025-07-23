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

RefConst<SphereShape> ShapeFactory::CreateSphereShape(const float_t radius) const
{
    const SphereShapeSettings shapeSettings(radius);
    shapeSettings.SetEmbedded();
    return RefConst(dynamic_cast<const SphereShape*>(shapeSettings.Create().Get().GetPtr()));
}

RefConst<BoxShape> ShapeFactory::CreateBoxShape(const glm::vec3 halfSize) const
{
    const BoxShapeSettings shapeSettings(Vec3(halfSize.x, halfSize.y, halfSize.z));
    shapeSettings.SetEmbedded();
    return RefConst(dynamic_cast<const BoxShape*>(shapeSettings.Create().Get().GetPtr()));
}

RefConst<CapsuleShape> ShapeFactory::CreateCapsuleShape(const float_t height, const float_t radius) const
{
    const CapsuleShapeSettings shapeSettings(height / 2.0f, radius);
    shapeSettings.SetEmbedded();
    return RefConst(dynamic_cast<const CapsuleShape*>(shapeSettings.Create().Get().GetPtr()));
}

}
