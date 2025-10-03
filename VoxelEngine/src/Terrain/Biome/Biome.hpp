//
// Created by RadU on 9/27/2025.
//

#pragma once

#include "PerlinNoise.hpp"
#include "../Voxel.hpp"
#include "../Structures/StructureGenerator.hpp"

namespace VoxelEngine
{

class Biome
{

public:
    explicit Biome(uint32_t seed);
    ~Biome() = default;

    VoxelType ResolveVoxelType(glm::i32vec3 globalPosition, int32_t height) const;
    int32_t GetHeight(int32_t x, int32_t z) const;
    void GenerateStructures(const Voxel (&surfaceLayer)[CHUNK_WIDTH][CHUNK_WIDTH],
                            Position2D chunkPosition,
                            std::vector<Structure>& output) const;

private:
    double_t GetDensity(glm::i32vec3 globalPosition, int32_t height) const;

private:
    siv::PerlinNoise m_Perlin;
    std::unique_ptr<StructureGenerator> m_Generator;
};

}
