//
// Created by RadU on 10/12/2025.
//

#pragma once

#include "../StructureGenerator.hpp"

namespace VoxelEngine
{

class DarkTreeGenerator final : public StructureGenerator
{

public:
    void Generate(const Context& context, std::vector<Structure>& output) const override;

};

}
