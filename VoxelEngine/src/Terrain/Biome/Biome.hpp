//
// Created by RadU on 9/27/2025.
//

#pragma once

#include <set>

#include "PerlinNoise.hpp"
#include "../Voxel.hpp"
#include "Structures/StructureGenerator.hpp"
#include "BiomeType.hpp"

namespace VoxelEngine
{

class Biome
{

public:
    explicit Biome(uint32_t seed);
    ~Biome() = default;

    struct GeneratorContext
    {
        const Voxel (&SurfaceLayer)[CHUNK_WIDTH][CHUNK_WIDTH];
        const Position2D ChunkPosition;
        const std::set<BiomeType>& ChunkBiomeTypes;

        GeneratorContext(const Voxel (&surfaceLayer)[CHUNK_WIDTH][CHUNK_WIDTH],
                         Position2D chunkPosition,
                         const std::set<BiomeType>& chunkBiomeTypes);
    };

    std::tuple<BiomeType, VoxelType> ResolveBiomeFeatures(glm::i32vec3 position, int32_t height) const;
    int32_t GetHeight(int32_t x, int32_t z) const;
    void GenerateStructures(const GeneratorContext& ctx, std::vector<Structure>& output) const;

private:
    double_t GetDensity(glm::i32vec3 position, int32_t height) const;
    BiomeType ResolveBiomeType(int32_t x, int32_t z) const;

private:
    const siv::PerlinNoise m_Perlin;
    const int32_t m_PerlinSeed;

    std::map<BiomeType, std::vector<std::unique_ptr<StructureGenerator>>> m_Generators;
};

}
