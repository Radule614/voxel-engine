//
// Created by RadU on 10/12/2025.
//

#include "ShrineFactory.hpp"

using namespace VoxelEngine;

namespace Sandbox
{

Structure ShrineFactory::CreateShrine(const Position3D position)
{
    static std::vector<std::pair<glm::i32vec3, VoxelType> > data{};

    if (data.empty())
    {
        for (size_t y = 1; y <= 3; ++y)
            data.emplace_back(glm::i32vec3(0, y, 0), SOUL);
    }

    Structure structure(SOUL, data);
    structure.SetRootPosition(position);

    return structure;
}

}
