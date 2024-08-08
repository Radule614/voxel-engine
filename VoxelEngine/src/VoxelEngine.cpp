#include "GLCore.hpp"
#include "Terrain/VoxelLayer.hpp"
#include "UserInterface.hpp"
#include "Physics/PhysicsLayer.hpp"
#include "Physics/PhysicsEngine.hpp"
#include "Ecs/Ecs.hpp"

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
		m_State.CameraController = GLCore::Utils::PerspectiveCameraController(45.0f, 16.0f / 9.0f, 150.0f);
		m_State.CameraController.GetCamera().SetPosition(glm::vec3(0.0f, CHUNK_HEIGHT, 0.0f));

		PushLayer(new VoxelLayer(m_State));
		PushLayer(new PhysicsLayer(m_State));
		PushOverlay(new UserInterface(m_State, *this));
	}

	void Init()
	{
		PhysicsEngine::Init();
		EntityComponentSystem::Init();
	}

	void Shutdown()
	{
		EntityComponentSystem::Shutdown();
		PhysicsEngine::Shutdown();
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