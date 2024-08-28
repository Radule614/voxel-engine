#include "ColliderComponent.hpp"

using namespace JPH;
using namespace JPH::literals;

namespace VoxelEngine
{

ColliderComponent::ColliderComponent(JPH::BodyID bodyId, JPH::Shape* shape) : m_BodyId(bodyId), m_Shape(shape)
{
}

ColliderComponent::~ColliderComponent()
{
}

JPH::BodyID ColliderComponent::GetBodyId() const
{
	return m_BodyId;
}

ColliderComponent ColliderFactory::CreateSphereCollider(float_t r, glm::vec3 p)
{
	PhysicsSystem& physicsSystem = PhysicsEngine::Instance().GetSystem();
	BodyInterface& bodyInterface = physicsSystem.GetBodyInterface();
	SphereShape* sphereShape = new SphereShape(r);
	BodyCreationSettings sphereSettings(sphereShape, RVec3(p.x, p.y, p.z), Quat::sIdentity(), EMotionType::Dynamic, Layers::MOVING);
	BodyID bodyId = bodyInterface.CreateAndAddBody(sphereSettings, EActivation::Activate);
	ColliderComponent collider(bodyId, sphereShape);
	return collider;
}

ColliderComponent ColliderFactory::CreateBoxCollider()
{
	PhysicsSystem& physicsSystem = PhysicsEngine::Instance().GetSystem();
	BodyInterface& bodyInterface = physicsSystem.GetBodyInterface();
	BoxShapeSettings boxShapeSettings(Vec3(0.5f, 0.5f, 0.5f));
	boxShapeSettings.SetEmbedded();
	ShapeSettings::ShapeResult boxShapeResult = boxShapeSettings.Create();
	ShapeRefC boxShape = boxShapeResult.Get();
	BodyCreationSettings boxSettings(boxShape, Vec3(0, 0, 0), Quat::sIdentity(), EMotionType::Static, Layers::NON_MOVING);
}

}