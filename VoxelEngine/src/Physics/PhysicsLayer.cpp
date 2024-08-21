#include "PhysicsLayer.hpp"
#include "PhysicsEngine.hpp"
#include "../Terrain/TerrainConfig.hpp"
#include "../Terrain/Voxel.hpp"
#include "../Terrain/VoxelMeshBuilder.hpp"
#include "../Assets/Model.hpp"
#include "../Ecs/MeshComponent.hpp"
#include "../Ecs/Ecs.hpp"

using namespace GLCore;
using namespace GLCore::Utils;
using namespace JPH;
using namespace JPH::literals;

static BodyID TestVoxelID;
static VoxelEngine::Voxel v(VoxelEngine::VoxelType::STONE);

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

	BodyCreationSettings voxelSettings(new BoxShape(Vec3(1.0f, 1.0f, 1.0f)), RVec3(CHUNK_WIDTH / 2, CHUNK_HEIGHT + 10.0_r, 5), Quat::sIdentity(), EMotionType::Dynamic, Layers::MOVING);
	BodyID voxelId = bodyInterface.CreateAndAddBody(voxelSettings, EActivation::Activate);
	bodyInterface.AddImpulse(voxelId, Vec3(200.0f, 0.0f, 0.0f), Vec3(30.0f, 0.0f, 0.0f));
	TestVoxelID = voxelId;

	physicsSystem.OptimizeBroadPhase();

	Model* model = m_State.AssetManager.LoadModel("assets/models/sphere/sphere.obj");
	LOG_INFO("MODEL MESHES SIZE: {0}", model->Meshes.size());
	auto& registry = EntityComponentSystem::Instance().GetEntityRegistry();
	const auto entity = registry.create();
	registry.emplace<MeshComponent>(entity, m_Shader, model->Meshes);
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
