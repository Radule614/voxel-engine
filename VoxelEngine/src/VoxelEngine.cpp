#include "VoxelEngine.hpp"

#define FMT_COMPILE

#include "GLCore.hpp"
#include "Terrain/VoxelLayer.hpp"
#include "UserInterface.hpp"
#include "Assets/AssetManager.hpp"
#include "Physics/PhysicsLayer.hpp"
#include "Physics/PhysicsEngine.hpp"
#include "Ecs/Ecs.hpp"
#include "Renderer/RendererLayer.hpp"

using namespace GLCore;

namespace VoxelEngine
{
VoxelEngineApp::VoxelEngineApp() : Application("Voxel Engine")
{
    m_State.Application = this;

    Init();
}

VoxelEngineApp::~VoxelEngineApp() { Shutdown(); }

void VoxelEngineApp::Run()
{
    Setup();
    Application::Run();
}

void VoxelEngineApp::Init()
{
    AssetManager::Init();
    PhysicsEngine::Init();
    EntityComponentSystem::Init();

    PushLayer(new PhysicsLayer(m_State));
    PushLayer(new RendererLayer(m_State));
    PushOverlay(new UserInterface(m_State));
}

void VoxelEngineApp::Shutdown()
{
    EntityComponentSystem::Shutdown();
    PhysicsEngine::Shutdown();
    AssetManager::Shutdown();
}

}
