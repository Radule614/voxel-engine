#pragma once

#include <GLCore.hpp>
#include "VoxelLayer.hpp"

using namespace GLCore;

class VoxelEngine : public Application
{
public:
    VoxelEngine() : Application("Voxel Engine")
    {
        PushLayer(new VoxelLayer());
    }
};
