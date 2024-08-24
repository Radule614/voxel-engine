#include "EcsLayer.hpp"
#include "../Physics/PhysicsEngine.hpp"
#include "../Ecs/MeshComponent.hpp"
#include "../Ecs/Ecs.hpp"
#include "../Renderer/Renderer.hpp"
#include "TransformComponent.hpp"
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
