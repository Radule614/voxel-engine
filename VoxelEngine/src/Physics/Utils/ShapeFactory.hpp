//
// Created by RadU on 09-Jun-25.
//

#pragma once

#include "../PhysicsEngine.hpp"
#include "Jolt/Physics/Collision/Shape/BoxShape.h"
#include "Jolt/Physics/Collision/Shape/CapsuleShape.h"
#include "Jolt/Physics/Collision/Shape/SphereShape.h"

namespace VoxelEngine
{

class ShapeFactory
{
public:
    ShapeFactory();
    ~ShapeFactory();

    JPH::RefConst<JPH::SphereShape> CreateSphereShape(float_t radius) const;
    JPH::RefConst<JPH::BoxShape> CreateBoxShape(glm::vec3 halfSize) const;
    JPH::RefConst<JPH::CapsuleShape> CreateCapsuleShape(float_t height, float_t radius) const;
};

}
