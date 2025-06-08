#include "GLCore.hpp"
#include "Terrain/VoxelLayer.hpp"
#include "UserInterface.hpp"
#include "Physics/PhysicsLayer.hpp"
#include "Physics/PhysicsEngine.hpp"
#include "Ecs/Ecs.hpp"
#include "Ecs/EcsLayer.hpp"
#include "Sandbox/SandboxLayer.hpp"

using namespace GLCore;

namespace VoxelEngine
{

class VoxelEngineApp : public Application
{
public:
	VoxelEngineApp() : Application("Voxel Engine")
	{
		Init();
		Setup();
	}

	~VoxelEngineApp()
	{
		Shutdown();
	}

private:
	void Setup()
	{
		m_State.Application = this;
		m_State.CameraController = Utils::PerspectiveCameraController(45.0f, 16.0f / 9.0f, 100.0f);
		m_State.CameraController.GetCamera().SetPosition(glm::vec3(0.0f, CHUNK_HEIGHT, 0.0f));

		DirectionalLight light = { glm::normalize(glm::vec3(1.0f, -2.0f, 1.0f)), glm::vec3(0.25f), glm::vec3(1.0f), glm::vec3(0.1f) };
		Renderer::Instance().SetDirectionalLight(light);

		PushLayer(new VoxelLayer(m_State));
		PushLayer(new PhysicsLayer(m_State));
		PushLayer(new EcsLayer(m_State));
		PushOverlay(new UserInterface(m_State));

		PushLayer(new Sandbox::SandboxLayer(m_State));
	}

	void Init()
	{
		AssetManager::Init();
		PhysicsEngine::Init();
		EntityComponentSystem::Init();
		Renderer::Init(GetWindow());
	}

	void Shutdown()
	{
		Renderer::Shutdown();
		EntityComponentSystem::Shutdown();
		PhysicsEngine::Shutdown();
		AssetManager::Shutdown();
	}

private:
	EngineState m_State;
};

}

int main()
{
	VoxelEngine::VoxelEngineApp app;
	app.Run();
}