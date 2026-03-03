//
// Created by RadU on 2/17/2026.
//

#pragma once

#include <string>

namespace VoxelEngine
{

struct ComponentGui
{
    virtual ~ComponentGui() = default;

    virtual std::string GetName() = 0;
    virtual void DrawGui() = 0;
};

}
