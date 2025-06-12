//
// Created by RadU on 12-Jun-25.
//

#pragma once

#include "GLCore/Utils/PerspectiveCameraController.hpp"

namespace VoxelEngine
{

struct CameraComponent
{
    CameraComponent(
        const std::shared_ptr<GLCore::Utils::PerspectiveCameraController>& controller): CameraController(controller)
    {
    }

    std::shared_ptr<GLCore::Utils::PerspectiveCameraController> CameraController;
};

}
