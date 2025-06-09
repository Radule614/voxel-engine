#pragma once
#include "Assets/AssetManager.hpp"
#include "GLCore/Core/Application.hpp"

namespace VoxelEngine
{

struct EngineState
{
    GLCore::Application* Application;

    bool MenuActive = false;
    GLCore::Utils::PerspectiveCameraController CameraController;
};

}
