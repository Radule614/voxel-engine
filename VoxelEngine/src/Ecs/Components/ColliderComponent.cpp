#include "ColliderComponent.hpp"

using namespace JPH;
using namespace JPH::literals;

namespace VoxelEngine
{

ColliderComponent::ColliderComponent(JPH::BodyID bodyId) : m_BodyId(bodyId)
{
}

ColliderComponent::~ColliderComponent()
{
}

JPH::BodyID ColliderComponent::GetBodyId() const
{
	return m_BodyId;
}

ColliderComponent ColliderFactory::CreateCollider(JPH::ShapeRefC shape, glm::vec3 p, EMotionType motion, EActivation activation)
{
	BodyInterface& bodyInterface = PhysicsEngine::Instance().GetSystem().GetBodyInterface();
	ObjectLayer layer = (motion == EMotionType::Static) ? Layers::NON_MOVING : Layers::MOVING;
	BodyCreationSettings bodySettings(shape, Vec3(p.x, p.y, p.z), Quat::sIdentity(), motion, layer);
	BodyID bodyId = bodyInterface.CreateAndAddBody(bodySettings, activation);
	ColliderComponent collider(bodyId);
	return collider;
}

ColliderComponent ColliderFactory::CreateSphereCollider(float_t r, glm::vec3 p, EMotionType motion, EActivation activation)
{
	SphereShapeSettings shapeSettings(r);
	shapeSettings.SetEmbedded();
	ShapeRefC shape = shapeSettings.Create().Get();
	return CreateCollider(shape, p, motion, activation);
}

ColliderComponent ColliderFactory::CreateBoxCollider(glm::vec3 s, glm::vec3 p, EMotionType motion, EActivation activation)
{
	BoxShapeSettings shapeSettings(Vec3(s.x, s.y, s.z));
	shapeSettings.SetEmbedded();
	ShapeRefC shape = shapeSettings.Create().Get();
	return CreateCollider(shape, p, motion, activation);
}

}