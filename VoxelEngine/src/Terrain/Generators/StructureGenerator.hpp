//
// Created by RadU on 9/21/2025.
//

#pragma once

#include "PerlinNoise.hpp"

#include "../Position2D.hpp"
#include "../TerrainConfig.hpp"
#include "../Structures/Structure.hpp"

namespace VoxelEngine
{

class StructureGenerator
{
public:
    StructureGenerator();
    virtual ~StructureGenerator() = default;

    struct GenerationContext
    {
        Position2D ChunkPosition{};
        const siv::PerlinNoise& PerlinEngine;
        const size_t (&HeightMap)[CHUNK_WIDTH][CHUNK_WIDTH];

        GenerationContext(Position2D chunkPosition,
                          const siv::PerlinNoise& perlin,
                          const size_t (&heightMap)[CHUNK_WIDTH][CHUNK_WIDTH]);
    };

    // Called during chunk generation
    virtual void Generate(const GenerationContext& context, std::vector<Structure>& output) const = 0;
};

}
