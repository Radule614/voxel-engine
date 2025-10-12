//
// Created by RadU on 10/12/2025.
//

#pragma once

#include "../Structure.hpp"

namespace VoxelEngine
{

class DarkTreeFactory
{
public:
    static Structure CreateTree(Position3D position);
    static Structure CreateLargeTree(Position3D position);

private:
    static void CreatePyramidShape(int32_t radius, int32_t y, int32_t step, std::vector<std::pair<glm::i32vec3, VoxelType> >& output);
};

}
