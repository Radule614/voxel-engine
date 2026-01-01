//
// Created by RadU on 10/12/2025.
//

#include "DarkTreeFactory.hpp"

namespace VoxelEngine
{

Structure DarkTreeFactory::CreateTree(const Position3D position)
{
    static std::vector<std::pair<glm::i32vec3, VoxelType> > data{};

    if (data.empty())
    {
        for (size_t y = 1; y <= 10; ++y)
            data.emplace_back(glm::i32vec3(0, y, 0), DARK_LOG);

        data.emplace_back(glm::i32vec3(0, 11, 0), DARK_LEAVES);

        CreatePyramidShape(2, 5, 2, data);
        CreatePyramidShape(1, 10, 1, data);
    }

    Structure structure(DARK_LOG, data);
    structure.SetRootPosition(position);

    return structure;
}

Structure DarkTreeFactory::CreateLargeTree(Position3D position)
{
    static std::vector<std::pair<glm::i32vec3, VoxelType> > data{};

    if (data.empty())
    {
        for (size_t y = 1; y <= 15; ++y)
            data.emplace_back(glm::i32vec3(0, y, 0), DARK_LOG);

        data.emplace_back(glm::i32vec3(0, 16, 0), DARK_LEAVES);

        CreatePyramidShape(3, 6, 2, data);
        CreatePyramidShape(1, 13, 1, data);
        CreatePyramidShape(2, 14, 1, data);
    }

    Structure structure(DARK_LOG, data);
    structure.SetRootPosition(position);

    return structure;
}

void DarkTreeFactory::CreatePyramidShape(
    const int32_t radius,
    int32_t y,
    const int32_t step,
    std::vector<std::pair<glm::i32vec3, VoxelType> >& output)
{
    for (int32_t r = radius; r >= 0; --r)
    {
        for (int32_t x = -r; x <= r; ++x)
        {
            for (int32_t z = -r; z <= r; ++z)
            {
                if (x == 0 && z == 0)
                    continue;

                for (int32_t s = 0; s < step; ++s)
                    output.emplace_back(glm::i32vec3(x, y + s, z), DARK_LEAVES);
            }
        }

        y += step;
    }
}

}
