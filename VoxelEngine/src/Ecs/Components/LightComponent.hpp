//
// Created by RadU on 1/1/2026.
//

#pragma once

#include "../../Renderer/PointLight.hpp"

namespace VoxelEngine
{

struct LightComponent
{
    PointLight PointLight;

    explicit LightComponent(const VoxelEngine::PointLight& pointLight) : PointLight(pointLight)
    {
    }
};

}
