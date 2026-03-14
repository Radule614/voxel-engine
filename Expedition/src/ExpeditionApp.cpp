#include "ExpeditionApp.hpp"
#include "World/WorldLayer.hpp"
#include "Terrain/VoxelLayer.hpp"
#include "Terrain/Biome/Biome.hpp"

namespace Expedition
{

using namespace VoxelEngine;

void ExpeditionApp::Setup()
{
    m_State.CameraController->SetFreeFly(false);
    m_State.CameraController->GetCamera().SetPosition({ 0.0f, 120.0f, 0.0f });
    m_State.CameraController->SetSpeed(15.0f);

    WorldSettings settings{};
    settings.m_Biome = std::make_unique<Biome>(6512u);

    auto* voxelLayer = new VoxelLayer(m_State);
    voxelLayer->Init(std::move(settings));
    PushLayer(voxelLayer);
    PushLayer(new WorldLayer(m_State));
}

}
