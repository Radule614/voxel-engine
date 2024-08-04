#include "GLCore.hpp"
#include "Terrain/VoxelLayer.hpp"
#include "UserInterface.hpp"
#include "Physics/PhysicsLayer.hpp"
#include "Physics/PhysicsEngine.hpp"

using namespace GLCore;

namespace VoxelEngine
{

class VoxelEngineApp : public Application
{
public:
	VoxelEngineApp() : Application("Voxel Engine")
	{
		VoxelEngine::PhysicsEngine::Init();

		PushLayer(new PhysicsLayer(m_State));
		PushLayer(new VoxelLayer(m_State));
		PushOverlay(new UserInterface(m_State, *this));
	}

	~VoxelEngineApp()
	{
		VoxelEngine::PhysicsEngine::Shutdown();
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