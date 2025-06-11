//
// Created by RadU on 09-Jun-25.
//

#pragma once

#include "../PhysicsEngine.hpp"

namespace VoxelEngine
{

class PhysicsFactory
{
public:
    PhysicsFactory();
    ~PhysicsFactory();

    JPH::BodyID CreateAndAddCollider(const JPH::ShapeRefC& shape,
                                     glm::vec3 position,
                                     JPH::EMotionType motion,
                                     JPH::EActivation activation) const;
    JPH::BodyID CreateAndAddSphereCollider(float_t radius,
                                           glm::vec3 position,
                                           JPH::EMotionType motion,
                                           JPH::EActivation activation) const;
    JPH::BodyID CreateAndAddBoxCollider(glm::vec3 halfSize,
                                        glm::vec3 position,
                                        JPH::EMotionType motion,
                                        JPH::EActivation activation) const;
    JPH::BodyID CreateAndAddCapsuleCollider(glm::vec3 position,
                                            float_t height,
                                            float_t radius,
                                            JPH::EMotionType motion,
                                            JPH::EActivation activation) const;
    JPH::ShapeRefC CreateBoxShape(glm::vec3 halfSize) const;

private:
    JPH::PhysicsSystem& m_PhysicsSystem;
};

}
