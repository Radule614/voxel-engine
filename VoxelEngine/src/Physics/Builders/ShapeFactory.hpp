//
// Created by RadU on 09-Jun-25.
//

#pragma once

#include "../PhysicsEngine.hpp"

namespace VoxelEngine
{

class ShapeFactory
{
public:
    ShapeFactory();
    ~ShapeFactory();

    JPH::ShapeRefC CreateSphereShape(float_t radius) const;
    JPH::ShapeRefC CreateBoxShape(glm::vec3 halfSize) const;
    JPH::ShapeRefC CreateCapsuleShape(float_t height, float_t radius) const;
};

}
