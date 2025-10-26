//
// Created by RadU on 10/26/2025.
//

#pragma once

#include "Terrain/Biome/Structures/Structure.hpp"

namespace Sandbox
{

class LampFactory
{
public:
    static VoxelEngine::Structure CreateLamp(VoxelEngine::Position3D position);
};

}
