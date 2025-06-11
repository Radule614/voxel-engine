//
// Created by RadU on 09-Jun-25.
//

#include "PhysicsFactory.hpp"
#include "Jolt/Physics/Collision/Shape/BoxShape.h"
#include "Jolt/Physics/Collision/Shape/CapsuleShape.h"
#include "Jolt/Physics/Collision/Shape/SphereShape.h"

using namespace JPH;

namespace VoxelEngine
{

PhysicsFactory::PhysicsFactory() : m_PhysicsSystem(PhysicsEngine::Instance().GetSystem())
{
}

PhysicsFactory::~PhysicsFactory() = default;

BodyID PhysicsFactory::CreateAndAddCollider(const ShapeRefC& shape,
                                            const glm::vec3 position,
                                            const EMotionType motion,
                                            const EActivation activation) const
{
    BodyInterface& bodyInterface = m_PhysicsSystem.GetBodyInterface();
    const ObjectLayer layer = motion == EMotionType::Static ? Layers::NON_MOVING : Layers::MOVING;
    const auto jphPosition = Vec3(position.x, position.y, position.z);
    const BodyCreationSettings bodySettings(shape, jphPosition, Quat::sIdentity(), motion, layer);
    return bodyInterface.CreateAndAddBody(bodySettings, activation);
}

BodyID PhysicsFactory::CreateAndAddSphereCollider(const float_t radius,
                                                  const glm::vec3 position,
                                                  const EMotionType motion,
                                                  const EActivation activation) const
{
    const SphereShapeSettings shapeSettings(radius);
    shapeSettings.SetEmbedded();
    const ShapeRefC shape = shapeSettings.Create().Get();
    return CreateAndAddCollider(shape, position, motion, activation);
}

BodyID PhysicsFactory::CreateAndAddBoxCollider(const glm::vec3 halfSize,
                                               const glm::vec3 position,
                                               const EMotionType motion,
                                               const EActivation activation) const
{
    const BoxShapeSettings shapeSettings(Vec3(halfSize.x, halfSize.y, halfSize.z));
    shapeSettings.SetEmbedded();
    const ShapeRefC shape = shapeSettings.Create().Get();
    return CreateAndAddCollider(shape, position, motion, activation);
}

BodyID PhysicsFactory::CreateAndAddCapsuleCollider(const glm::vec3 position,
                                                   const float_t height,
                                                   const float_t radius,
                                                   const EMotionType motion,
                                                   const EActivation activation) const
{
    const CapsuleShapeSettings shapeSettings(height, radius);
    shapeSettings.SetEmbedded();
    const ShapeRefC shape = shapeSettings.Create().Get();
    return CreateAndAddCollider(shape, position, motion, activation);
}

ShapeRefC PhysicsFactory::CreateBoxShape(const glm::vec3 halfSize) const
{
    const BoxShapeSettings shapeSettings(Vec3(halfSize.x, halfSize.y, halfSize.z));
    shapeSettings.SetEmbedded();
    return shapeSettings.Create().Get();
}

}
