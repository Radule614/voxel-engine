//
// Created by RadU on 9/21/2025.
//

#pragma once

#include "PerlinNoise.hpp"

#include "../../../Config.hpp"
#include "../../Utils/Position2D.hpp"
#include "Structure.hpp"

namespace VoxelEngine
{

class StructureGenerator
{
public:
    StructureGenerator();
    virtual ~StructureGenerator() = default;

    struct Context
    {
        const siv::PerlinNoise& Perlin;
        const int32_t PerlinSeed;
        const Voxel (&SurfaceLayer)[CHUNK_WIDTH][CHUNK_WIDTH];
        const Position2D ChunkPosition;

        Context(const siv::PerlinNoise& perlin,
                          int32_t perlinSeed,
                          const Voxel (&surfaceLayer)[CHUNK_WIDTH][CHUNK_WIDTH],
                          Position2D chunkPosition);
    };

    virtual void Generate(const Context& context, std::vector<Structure>& output) const = 0;
};

}
