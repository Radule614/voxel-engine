//
// Created by RadU on 2/17/2026.
//

#pragma once

#include "entt.hpp"
#include "../ComponentGui.hpp"
#include "../../Assets/Animation.hpp"

namespace VoxelEngine
{

struct EntityAnimation
{
    std::string Name;
    bool IsActive = false;
    float_t Time = 0.0f;
    float_t Duration = 0.0f;

    std::unordered_map<entt::entity, NodeAnimation> NodeAnimations{};
};

struct AnimationComponent : ComponentGui
{
    std::vector<EntityAnimation> Animations{};

    AnimationComponent() = default;
    ~AnimationComponent() override = default;

    std::string GetName() override;
    void DrawGui() override;
};

}
