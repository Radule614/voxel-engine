//
// Created by RadU on 10/14/2025.
//

#include "SandboxApp.hpp"
#include "Balls/BallLayer.hpp"
#include "Sponza/SponzaLayer.hpp"
#include "Structures/Shrine/ShrineGenerator.hpp"
#include "Terrain/VoxelLayer.hpp"

namespace Sandbox
{
using namespace VoxelEngine;

void SandboxApp::Setup()
{
    constexpr auto cameraPosition = glm::vec3(-140.5f * CHUNK_WIDTH, CHUNK_HEIGHT - 24, 30.5f * CHUNK_WIDTH);

    auto cameraController = std::make_shared<Utils::PerspectiveCameraController>(45.0f, 16.0f / 9.0f, 10.0f);
    cameraController->SetFreeFly(true);
    cameraController->GetCamera().SetPosition(cameraPosition);
    m_State.CameraController = std::move(cameraController);

    // WorldSettings settings{};
    // settings.m_Biome = std::make_unique<Biome>(6512u);
    // settings.m_Biome->AddGenerator(PLAINS, std::make_unique<ShrineGenerator>());
    //
    // auto* voxelLayer = new VoxelLayer(m_State);
    // voxelLayer->Init(std::move(settings));
    //
    // PushLayer(voxelLayer);
    // PushLayer(new BallLayer(m_State));

    PushLayer(new SponzaLayer(m_State));
}

}
