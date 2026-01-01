//
// Created by RadU on 10/11/2025.
//

#pragma once

#include "../StructureGenerator.hpp"

namespace VoxelEngine
{

class CactusGenerator final : public StructureGenerator
{
public:
    void Generate(const Context& context, std::vector<Structure>& output) const override;

};

}
