#pragma once

#include "GLCore/Core/Application.hpp"

namespace VoxelEngine
{

struct EngineState
{
    GLCore::Application* Application;

    bool MenuActive = false;
    std::shared_ptr<GLCore::Utils::PerspectiveCameraController> CameraController;
};

}
