#pragma once

#include <GLCore.hpp>
#include "Terrain/VoxelLayer.hpp"

using namespace GLCore;

class VoxelEngine : public Application
{
public:
    VoxelEngine() : Application("Voxel Engine")
    {
        PushLayer(new Terrain::VoxelLayer());
    }
};
