//
// Created by RadU on 9/27/2025.
//

#include "TreeFactory.hpp"

namespace VoxelEngine
{

Structure TreeFactory::CreateTree(const Position3D position)
{
    static std::vector<std::pair<glm::i32vec3, VoxelType> > data{};

    if (data.empty())
    {
        for (size_t y = 1; y <= 7; ++y)
            data.emplace_back(glm::i32vec3(0, y, 0), LOG);
        for (int32_t x = -3; x <= 3; ++x)
        {
            for (int32_t z = -3; z <= 3; ++z)
            {
                for (size_t y = 5; y <= 9; ++y)
                {
                    if (x == 0 && z == 0 && y <= 7)
                        continue;
                    if ((x == -3 || x == 3 || z == -3 || z == 3) && (y == 5 || y == 9))
                        continue;
                    data.emplace_back(glm::i32vec3(x, y, z), LEAVES);
                }
            }
        }
    }

    Structure structure(LOG, data);
    structure.SetRootPosition(position);

    return structure;
}

Structure TreeFactory::CreateLargeTree(const Position3D position)
{
    static std::vector<std::pair<glm::i32vec3, VoxelType> > data{};

    if (data.empty())
    {
        for (size_t y = 1; y <= 10; ++y)
            data.emplace_back(glm::i32vec3(0, y, 0), LOG);
        for (int32_t x = -3; x <= 3; ++x)
        {
            for (int32_t z = -3; z <= 3; ++z)
            {
                for (size_t y = 8; y <= 13; ++y)
                {
                    if (x == 0 && z == 0 && y <= 10)
                        continue;
                    if ((x == -3 || x == 3 || z == -3 || z == 3) && (y == 8 || y == 13))
                        continue;
                    data.emplace_back(glm::i32vec3(x, y, z), LEAVES);
                }
            }
        }
    }

    Structure structure(LOG, data);
    structure.SetRootPosition(position);

    return structure;
}

}
