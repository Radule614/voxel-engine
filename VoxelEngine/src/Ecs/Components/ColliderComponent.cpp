#include "ColliderComponent.hpp"

using namespace JPH;
using namespace JPH::literals;

namespace VoxelEngine
{

ColliderComponent::ColliderComponent(JPH::BodyID bodyId) : m_BodyId(bodyId)
{
}

ColliderComponent::~ColliderComponent() = default;

JPH::BodyID ColliderComponent::GetBodyId() const
{
    return m_BodyId;
}

ColliderComponent ColliderFactory::CreateCollider(const JPH::ShapeRefC& shape,
                                                  const glm::vec3 p,
                                                  const EMotionType motion,
                                                  const EActivation activation)
{
    BodyInterface& bodyInterface = PhysicsEngine::Instance().GetSystem().GetBodyInterface();
    const ObjectLayer layer = (motion == EMotionType::Static) ? Layers::NON_MOVING : Layers::MOVING;
    const BodyCreationSettings bodySettings(shape, Vec3(p.x, p.y, p.z), Quat::sIdentity(), motion, layer);
    const BodyID bodyId = bodyInterface.CreateAndAddBody(bodySettings, activation);
    ColliderComponent collider(bodyId);
    return collider;
}

ColliderComponent ColliderFactory::CreateSphereCollider(float_t r,
                                                        glm::vec3 p,
                                                        EMotionType motion,
                                                        EActivation activation)
{
    SphereShapeSettings shapeSettings(r);
    shapeSettings.SetEmbedded();
    ShapeRefC shape = shapeSettings.Create().Get();
    return CreateCollider(shape, p, motion, activation);
}

ColliderComponent ColliderFactory::CreateBoxCollider(glm::vec3 s,
                                                     glm::vec3 p,
                                                     EMotionType motion,
                                                     EActivation activation)
{
    BoxShapeSettings shapeSettings(Vec3(s.x, s.y, s.z));
    shapeSettings.SetEmbedded();
    ShapeRefC shape = shapeSettings.Create().Get();
    return CreateCollider(shape, p, motion, activation);
}

}
