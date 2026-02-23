//
// Created by RadU on 2/23/2026.
//

#pragma once

#include "entt.hpp"
#include "glm/glm.hpp"

namespace VoxelEngine
{

struct SkinComponent
{
    std::vector<entt::entity> JointEntities{};
    std::vector<glm::mat4> InverseBindMatrices{};

    std::vector<glm::mat4> JointMatrices{};
};

}
