//
// Created by RadU on 9/21/2025.
//

#pragma once

#include "../StructureGenerator.hpp"

namespace VoxelEngine
{

class TreeGenerator final : public StructureGenerator
{

public:
    void Generate(const Context& context, std::vector<Structure>& output) const override;

};

};
