//
// Created by RadU on 2/23/2026.
//

#pragma once

#include "entt.hpp"
#include "../ComponentGui.hpp"
#include "glm/glm.hpp"

namespace VoxelEngine
{

struct SkinComponent : ComponentGui
{
    bool IsEnabled = true;
    std::vector<entt::entity> JointEntities{};
    std::vector<glm::mat4> InverseBindMatrices{};
    std::vector<glm::mat4> JointMatrices{};

    SkinComponent() = default;
    ~SkinComponent() override = default;

    std::string GetName() override;
    void DrawGui() override;
};

}
