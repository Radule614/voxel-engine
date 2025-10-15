//
// Created by RadU on 10/14/2025.
//

#include "SandboxApp.hpp"
#include "Balls/BallLayer.hpp"
#include "Terrain/VoxelLayer.hpp"

namespace Sandbox
{
using namespace VoxelEngine;

void SandboxApp::Setup()
{
    auto cameraController = std::make_shared<Utils::PerspectiveCameraController>(45.0f, 16.0f / 9.0f, 100.0f);
    cameraController->SetFreeFly(true);
    cameraController->GetCamera().SetPosition(glm::vec3(0.0f, CHUNK_HEIGHT, 0.0f));
    m_State.CameraController = std::move(cameraController);

    WorldSettings settings{};
    settings.m_Biome = std::make_unique<Biome>(6512u);

    auto* voxelLayer = new VoxelLayer(m_State);
    voxelLayer->Init(std::move(settings));

    PushLayer(voxelLayer);

    PushLayer(new BallLayer(m_State));
}

}
