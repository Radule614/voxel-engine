//
// Created by RadU on 2/21/2026.
//

#pragma once

#include "glm/glm.hpp"
#include "glm/detail/type_quat.hpp"

namespace VoxelEngine
{

enum class AnimationInterpolation
{
    Linear,
    Step,
    CubicSpline
};

enum class AnimationTarget
{
    Translation,
    Rotation,
    Scale,
    Morph
};

struct AnimationTrack
{
    AnimationTarget Target;
    AnimationInterpolation Interpolation;

    std::vector<float_t> Times{};

    std::vector<glm::vec3> VectorValues{};
    std::vector<glm::quat> QuatValues{};
};

struct NodeAnimation
{
    std::vector<AnimationTrack> Tracks{};
};

struct Animation
{
    std::string Name;
    std::unordered_map<int32_t, NodeAnimation> NodeAnimations{};
};

}
