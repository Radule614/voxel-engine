//
// Created by RadU on 9/21/2025.
//

#pragma once

#include "PerlinNoise.hpp"

#include "../TerrainConfig.hpp"
#include "../Position2D.hpp"
#include "Structure.hpp"

namespace VoxelEngine
{

class StructureGenerator
{
public:
    StructureGenerator();
    virtual ~StructureGenerator() = default;

    struct GenerationContext
    {
        const siv::PerlinNoise& Perlin;
        const Voxel (&SurfaceLayer)[CHUNK_WIDTH][CHUNK_WIDTH];
        const Position2D ChunkPosition;

        GenerationContext(const siv::PerlinNoise& perlin,
                          const Voxel (&surfaceLayer)[CHUNK_WIDTH][CHUNK_WIDTH],
                          Position2D chunkPosition);
    };

    virtual void Generate(const GenerationContext& context, std::vector<Structure>& output) const = 0;
};

}
