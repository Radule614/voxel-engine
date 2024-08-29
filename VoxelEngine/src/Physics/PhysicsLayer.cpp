#include "PhysicsLayer.hpp"
#include "PhysicsEngine.hpp"
#include "../Terrain/TerrainConfig.hpp"
#include "../Terrain/Voxel.hpp"
#include "../Terrain/VoxelMeshBuilder.hpp"
#include "../Assets/Model.hpp"
#include "../Ecs/Ecs.hpp"

using namespace GLCore;
using namespace GLCore::Utils;
using namespace JPH;

static entt::entity SphereEntity;

namespace VoxelEngine
{

PhysicsLayer::PhysicsLayer(EngineState& state) : m_State(state)
{
	m_Shader = std::make_shared<Shader>(*Shader::FromGLSLTextFiles("assets/shaders/default.vert.glsl", "assets/shaders/default.frag.glsl"));
	m_TextureAtlas = m_State.AssetManager.LoadTexture("assets/textures/atlas.png", "texture_diffuse");
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

	PhysicsSystem& physicsSystem = PhysicsEngine::Instance().GetSystem();
	BodyInterface& bodyInterface = physicsSystem.GetBodyInterface();
	auto& registry = EntityComponentSystem::Instance().GetEntityRegistry();

	ColliderComponent collider = ColliderFactory::CreateSphereCollider(1.0f, glm::vec3(8, 25, 8), EMotionType::Dynamic, EActivation::Activate);
	bodyInterface.AddImpulse(collider.GetBodyId(), Vec3(300.0f, 0.0f, 0.0f), Vec3(70.0f, 150.0f, 50.0f));
	physicsSystem.OptimizeBroadPhase();

	Model* model = m_State.AssetManager.LoadModel("assets/models/moon/moon.obj");

	const auto entity = registry.create();
	registry.emplace<MeshComponent>(entity, m_Shader, model->Meshes);
	registry.emplace<TransformComponent>(entity);
	registry.emplace<ColliderComponent>(entity, collider);
	SphereEntity = entity;
}

void PhysicsLayer::OnEvent(GLCore::Event& event)
{
	if (!m_State.MenuActive)
		m_State.CameraController.OnEvent(event);
}

void PhysicsLayer::OnUpdate(GLCore::Timestep ts)
{
	PhysicsEngine::Instance().OnUpdate(ts);
	if (!m_State.MenuActive)
		m_State.CameraController.OnUpdate(ts);
}

}
