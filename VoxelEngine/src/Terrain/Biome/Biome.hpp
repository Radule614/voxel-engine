//
// Created by RadU on 9/27/2025.
//

#pragma once

#include "PerlinNoise.hpp"
#include "../Voxel.hpp"

namespace VoxelEngine
{

class Biome
{

public:
    explicit Biome(uint32_t heightSeed, uint32_t densitySeed);
    ~Biome() = default;

    VoxelType ResolveVoxelType(glm::i32vec3 globalPosition, int32_t height) const;
    int32_t GetHeight(int32_t x, int32_t z) const;

private:
    double_t GetDensity(glm::i32vec3 globalPosition, int32_t height) const;

private:
    siv::PerlinNoise m_HeightPerlin;
    siv::PerlinNoise m_DensityPerlin;
};

}
