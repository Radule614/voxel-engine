//
// Created by RadU on 10/15/2025.
//

#pragma once

#include <memory>
#include "Terrain/Biome/Biome.hpp"

namespace VoxelEngine
{

struct WorldSettings
{
    std::unique_ptr<Biome> m_Biome;
};

}
