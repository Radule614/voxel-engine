#include "EcsLayer.hpp"
#include "../Physics/PhysicsEngine.hpp"
#include "../Ecs/Ecs.hpp"
#include "../Renderer/Renderer.hpp"
#include "../Utils/Utils.hpp"

using namespace GLCore;
using namespace GLCore::Utils;
using namespace JPH;

namespace VoxelEngine
{

EcsLayer::EcsLayer(EngineState& state) : m_State(state)
{
}

EcsLayer::~EcsLayer()
{
}

void EcsLayer::OnAttach()
{
	EnableGLDebugging();
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_FRONT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void EcsLayer::OnDetach()
{
}

void EcsLayer::OnUpdate(GLCore::Timestep ts)
{
	PhysicsSystem& physicsSystem = PhysicsEngine::Instance().GetSystem();
	BodyInterface& bodyInterface = physicsSystem.GetBodyInterface();
	auto& registry = EntityComponentSystem::Instance().GetEntityRegistry();
	auto colliderView = registry.view<ColliderComponent>();
	for (auto entity : colliderView)
	{
		auto& collider = colliderView.get<ColliderComponent>(entity);
		auto bodyId = collider.GetBodyId();
		if (!bodyInterface.IsActive(bodyId))
			continue;
		RVec3 p = bodyInterface.GetCenterOfMassPosition(bodyId);
		Vec3 r;
		float_t angle;
		bodyInterface.GetRotation(bodyId).GetAxisAngle(r, angle);
		auto& transform = registry.view<TransformComponent>().get<TransformComponent>(entity);
		transform.PreviousPosition = transform.Position;
		transform.Position = glm::vec3(p.GetX(), p.GetY(), p.GetZ());
		transform.RotationAngle = angle;
		if (r != Vec3(0.0f, 0.0f, 0.0f))
			transform.RotationAxis = glm::vec3(r.GetX(), r.GetY(), r.GetZ());
		else
			transform.RotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
		if ((glm::ivec3)glm::round(transform.PreviousPosition) != (glm::ivec3)glm::round(transform.Position))
		{
			ColliderLocationChangedEvent event(transform.Position);
			m_State.Application->RaiseEvent(event);
		}
	}
	Renderer::Instance().RenderScene(m_State.CameraController.GetCamera());
}

}
