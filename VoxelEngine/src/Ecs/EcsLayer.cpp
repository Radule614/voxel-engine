#include "EcsLayer.hpp"
#include "../Physics/PhysicsEngine.hpp"
#include "../Ecs/Ecs.hpp"
#include "../Renderer/Renderer.hpp"
#include "../Utils/Utils.hpp"

using namespace GLCore;
using namespace GLCore::Utils;
using namespace JPH;
using namespace JPH::literals;

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
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void EcsLayer::OnDetach()
{
}

void EcsLayer::OnUpdate(GLCore::Timestep ts)
{
	glCullFace(GL_BACK);
	auto& registry = EntityComponentSystem::Instance().GetEntityRegistry();
	PhysicsSystem& physicsSystem = PhysicsEngine::Instance().GetSystem();
	BodyInterface& bodyInterface = physicsSystem.GetBodyInterface();

	auto colliderView = registry.view<ColliderComponent>();
	for (auto entity : colliderView)
	{
		auto& collider = colliderView.get<ColliderComponent>(entity);
		auto bodyId = collider.GetBodyId();
		if (!bodyInterface.IsActive(bodyId))
			continue;
		bodyInterface.GetTransformedShape(bodyId).GetShapeScale();
		RVec3 p = bodyInterface.GetCenterOfMassPosition(bodyId);
		Vec3 r;
		float_t angle;
		bodyInterface.GetRotation(bodyId).GetAxisAngle(r, angle);
		TransformComponent& transform = registry.view<TransformComponent>().get<TransformComponent>(entity);
		transform.Position = glm::vec3(p.GetX(), p.GetY(), p.GetZ());
		transform.RotationAngle = angle;
		transform.RotationAxis = glm::vec3(r.GetX(), r.GetY(), r.GetZ());
		transform.Scale = glm::vec3(1);
	}

	auto view = registry.view<MeshComponent, TransformComponent>();
	for (auto entity : view)
	{
		auto& mesh = view.get<MeshComponent>(entity);
		auto& transform = view.get<TransformComponent>(entity);
		glm::mat4 model = glm::mat4(1);
		model = glm::translate(model, transform.Position);
		model = glm::rotate(model, transform.RotationAngle, transform.RotationAxis);
		model = glm::scale(model, transform.Scale);
		Renderer::Instance().Render(mesh, m_State.CameraController.GetCamera(), model);
	}

	glCullFace(GL_FRONT);
}

}
