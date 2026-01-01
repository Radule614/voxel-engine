//
// Created by RadU on 10/12/2025.
//

#pragma once

#include "Terrain/Biome/Structures/StructureGenerator.hpp"

namespace Sandbox
{

class ShrineGenerator final : public VoxelEngine::StructureGenerator
{
public:
    void Generate(const Context& context, std::vector<VoxelEngine::Structure>& output) const override;

};

}
