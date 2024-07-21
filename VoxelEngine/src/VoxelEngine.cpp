#include "GLCore.hpp"
#include "Terrain/VoxelLayer.hpp"
#include "UserInterface.hpp"

using namespace GLCore;

namespace VoxelEngine
{

class VoxelEngine : public Application
{
public:
	VoxelEngine() : Application("Voxel Engine")
	{
		PushOverlay(new UserInterface(m_State, *this));
		PushLayer(new VoxelLayer(m_State));
	}
private:
	EngineState m_State;
};
}

int main()
{
	VoxelEngine::VoxelEngine app;
	app.Run();
}