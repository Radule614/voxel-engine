//
// Created by RadU on 15-Jun-25.
//

#pragma once

#include "../PhysicsEngine.hpp"

namespace VoxelEngine
{

class JoltUtils
{
public:
    static glm::vec3 JoltToGlmVec3(const JPH::Vec3 v) { return glm::vec3(v.GetX(), v.GetY(), v.GetZ()); }

    static JPH::Vec3 GlmToJoltVec3(const glm::vec3 v) { return JPH::Vec3(v.x, v.y, v.z); }
};

}
