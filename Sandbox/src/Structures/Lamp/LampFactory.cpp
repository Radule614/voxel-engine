//
// Created by RadU on 10/26/2025.
//

#include "LampFactory.hpp"

using namespace VoxelEngine;

namespace Sandbox
{

Structure LampFactory::CreateLamp(const Position3D position)
{
    static std::vector<std::pair<glm::i32vec3, VoxelType> > data{};

    if (data.empty())
    {
        for (size_t y = 1; y <= 3; ++y)
            data.emplace_back(glm::i32vec3(0, y, 0), LOG);

        data.emplace_back(glm::i32vec3(0, 4, 0), LAMP);
    }

    Structure structure(LOG, data);
    structure.SetRootPosition(position);

    return structure;
}

}
