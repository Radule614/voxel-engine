//
// Created by RadU on 10/14/2025.
//

#include "SandboxApp.hpp"
#include "Balls/BallLayer.hpp"
#include "GLFW/include/GLFW/glfw3.h"
#include "Terrain/VoxelLayer.hpp"

namespace Sandbox
{
using namespace VoxelEngine;

void SandboxApp::Setup()
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    auto cameraController = std::make_shared<Utils::PerspectiveCameraController>(45.0f, 16.0f / 9.0f, 100.0f);
    cameraController->SetFreeFly(true);
    cameraController->GetCamera().SetPosition(glm::vec3(CHUNK_WIDTH / 2, CHUNK_HEIGHT - 24, CHUNK_WIDTH / 2));
    m_State.CameraController = std::move(cameraController);

    WorldSettings settings{};
    settings.m_Biome = std::make_unique<Biome>(6512u);

    auto* voxelLayer = new VoxelLayer(m_State);
    voxelLayer->Init(std::move(settings));

    PushLayer(voxelLayer);
    PushLayer(new BallLayer(m_State));
}

}
