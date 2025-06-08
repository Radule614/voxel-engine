#include "SandboxLayer.hpp"
#include "../Physics/PhysicsEngine.hpp"

using namespace GLCore;
using namespace GLCore::Utils;
using namespace JPH;
using namespace VoxelEngine;

namespace Sandbox
{

SandboxLayer::SandboxLayer(EngineState& state) : m_State(state)
{
}

SandboxLayer::~SandboxLayer() = default;

void SandboxLayer::OnAttach()
{
	EnableGLDebugging();
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_FRONT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	m_Shader = std::make_shared<Shader>(*Shader::FromGLSLTextFiles("assets/shaders/default.vert.glsl", "assets/shaders/default.frag.glsl"));
	m_TextureAtlas = AssetManager::Instance().LoadTexture("assets/textures/atlas.png", "Diffuse");
	m_Model = AssetManager::Instance().LoadModel("assets/models/sphere/sphere.obj");
}

void SandboxLayer::OnEvent(GLCore::Event& event)
{
	EventDispatcher dispatcher(event);
	dispatcher.Dispatch<KeyPressedEvent>(
		[&](KeyPressedEvent& e)
		{
			if (e.GetKeyCode() == HZ_KEY_T)
			{
				const glm::vec3 front = m_State.CameraController.GetCamera().GetFront();
				const glm::vec3 position = m_State.CameraController.GetCamera().GetPosition();

				PhysicsSystem& physicsSystem = PhysicsEngine::Instance().GetSystem();
				BodyInterface& bodyInterface = physicsSystem.GetBodyInterface();
				auto& registry = EntityComponentSystem::Instance().GetEntityRegistry();

				ColliderComponent collider = ColliderFactory::CreateSphereCollider(0.4f, position, EMotionType::Dynamic, EActivation::Activate);
				bodyInterface.AddLinearVelocity(collider.GetBodyId(), 20 * Vec3(front.x, front.y, front.z));
				TransformComponent transform{};
				transform.Scale = glm::vec3(0.4);

				const auto entity = registry.create();
				registry.emplace<MeshComponent>(entity, m_Shader, m_Model->Meshes);
				registry.emplace<TransformComponent>(entity, transform);
				registry.emplace<ColliderComponent>(entity, collider);
				m_SphereEntities.emplace_back(entity, 0);
			}
			return false;
		});
}

void SandboxLayer::OnUpdate(const GLCore::Timestep ts)
{
	for (auto it = m_SphereEntities.begin(); it != m_SphereEntities.end();)
	{
		entt::entity& sphere = it->first;
		float_t& accumulatedTime = it->second;
		accumulatedTime += ts;
		if (accumulatedTime > 10.0f)
		{
			BodyInterface& bodyInterface = PhysicsEngine::Instance().GetSystem().GetBodyInterface();
			auto& registry = EntityComponentSystem::Instance().GetEntityRegistry();
			auto& collider = registry.view<ColliderComponent>().get<ColliderComponent>(sphere);
			bodyInterface.RemoveBody(collider.GetBodyId());
			bodyInterface.DestroyBody(collider.GetBodyId());
			registry.destroy(sphere);
			it = m_SphereEntities.erase(it);
		}
		else
			++it;
	}
}

}