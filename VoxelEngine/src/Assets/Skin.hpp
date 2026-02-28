//
// Created by RadU on 2/24/2026.
//

#pragma once

#include <glm/glm.hpp>

namespace VoxelEngine
{

struct Skin
{
    int32_t NodeIndex = -1;
    std::vector<int32_t> JointIndexes{};
    std::vector<glm::mat4> InverseBindMatrices{};
};

}
