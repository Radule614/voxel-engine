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

class ColliderFactory
{
public:
    static ColliderComponent CreateCollider(const JPH::ShapeRefC& shape,
                                            glm::vec3 p,
                                            JPH::EMotionType motion,
                                            JPH::EActivation activation);
    static ColliderComponent CreateSphereCollider(float_t r,
                                                  glm::vec3 p,
                                                  JPH::EMotionType motion,
                                                  JPH::EActivation activation);
    static ColliderComponent CreateBoxCollider(glm::vec3 s,
                                               glm::vec3 p,
                                               JPH::EMotionType motion,
                                               JPH::EActivation activation);
};

}
