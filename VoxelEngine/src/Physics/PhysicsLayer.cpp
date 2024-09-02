#include "PhysicsLayer.hpp"
#include "PhysicsEngine.hpp"
#include "../Terrain/TerrainConfig.hpp"
#include "../Terrain/Voxel.hpp"
#include "../Terrain/VoxelMeshBuilder.hpp"
#include "../Ecs/Ecs.hpp"
#include "GlCore.hpp"

using namespace GLCore;
using namespace GLCore::Utils;
using namespace JPH;

static std::vector<std::pair<entt::entity, float_t>> SphereEntities{};

namespace VoxelEngine
{

PhysicsLayer::PhysicsLayer(EngineState& state) : m_State(state)
{
	m_Shader = std::make_shared<Shader>(*Shader::FromGLSLTextFiles("assets/shaders/default.vert.glsl", "assets/shaders/default.frag.glsl"));
	m_TextureAtlas = m_State.AssetManager.LoadTexture("assets/textures/atlas.png", "texture_diffuse");
	m_Model = m_State.AssetManager.LoadModel("assets/models/moon/moon.obj");
}

PhysicsLayer::~PhysicsLayer()
{
}

void PhysicsLayer::OnAttach()
{
	EnableGLDebugging();
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_FRONT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void PhysicsLayer::OnEvent(GLCore::Event& event)
{
	if (!m_State.MenuActive)
		m_State.CameraController.OnEvent(event);

	EventDispatcher dispatcher(event);
	dispatcher.Dispatch<KeyPressedEvent>(
		[&](KeyPressedEvent& e)
		{
			if (e.GetKeyCode() == HZ_KEY_T)
			{
				glm::vec3 front = m_State.CameraController.GetCamera().GetFront();
				glm::vec3 position = m_State.CameraController.GetCamera().GetPosition();

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
				SphereEntities.push_back(std::make_pair(entity, 0));
			}
			return false;
		});
}

void PhysicsLayer::OnUpdate(GLCore::Timestep ts)
{
	PhysicsEngine::Instance().OnUpdate(ts);
	if (!m_State.MenuActive)
		m_State.CameraController.OnUpdate(ts);
	for (auto it = SphereEntities.begin(); it != SphereEntities.end();)
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
			it = SphereEntities.erase(it);
		}
		else
			++it;
	}
}

}
