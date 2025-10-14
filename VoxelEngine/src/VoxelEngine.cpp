#include "VoxelEngine.hpp"

#include "GLCore.hpp"
#include "Terrain/VoxelLayer.hpp"
#include "UserInterface.hpp"
#include "Assets/AssetManager.hpp"
#include "Physics/PhysicsLayer.hpp"
#include "Physics/PhysicsEngine.hpp"
#include "Ecs/Ecs.hpp"
#include "Ecs/EcsLayer.hpp"
#include "Sandbox/SandboxLayer.hpp"

using namespace GLCore;

namespace VoxelEngine
{
VoxelEngineApp::VoxelEngineApp() : Application("Voxel Engine")
{
    Init();
    Setup();
}

VoxelEngineApp::~VoxelEngineApp() { Shutdown(); }

void VoxelEngineApp::Setup()
{
    m_State.Application = this;

    auto cameraController = std::make_shared<Utils::PerspectiveCameraController>(45.0f, 16.0f / 9.0f, 100.0f);
    cameraController->SetFreeFly(true);
    cameraController->GetCamera().SetPosition(glm::vec3(0.0f, CHUNK_HEIGHT, 0.0f));
    m_State.CameraController = std::move(cameraController);

    const World::Settings settings{};

    auto* voxelLayer = new VoxelLayer(m_State);
    voxelLayer->Init(settings);

    PushLayer(voxelLayer);
    PushLayer(new PhysicsLayer(m_State));
    PushLayer(new EcsLayer(m_State));
    PushOverlay(new UserInterface(m_State));

    PushLayer(new Sandbox::SandboxLayer(m_State));
}

void VoxelEngineApp::Init()
{
    AssetManager::Init();
    PhysicsEngine::Init();
    EntityComponentSystem::Init();
    Renderer::Init(GetWindow());
}

void VoxelEngineApp::Shutdown()
{
    Renderer::Shutdown();
    EntityComponentSystem::Shutdown();
    PhysicsEngine::Shutdown();
    AssetManager::Shutdown();
}

}
