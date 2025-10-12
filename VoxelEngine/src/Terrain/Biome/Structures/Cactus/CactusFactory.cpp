//
// Created by RadU on 10/11/2025.
//

#include "CactusFactory.hpp"

namespace VoxelEngine
{

Structure CactusFactory::CreateCactus(const Position3D position)
{
    static std::vector<std::pair<glm::i32vec3, VoxelType> > data{};

    if (data.empty())
    {
        for (size_t y = 1; y <= 4; ++y)
            data.emplace_back(glm::i32vec3(0, y, 0), CACTUS);
    }

    Structure structure(CACTUS, data);
    structure.SetRootPosition(position);

    return structure;
}

}
