//
// Created by RadU on 9/23/2025.
//

#include "StructureGenerator.hpp"

namespace VoxelEngine
{

StructureGenerator::StructureGenerator() = default;

StructureGenerator::GenerationContext::GenerationContext(
    const Position2D chunkPosition,
    const siv::PerlinNoise& perlin,
    const size_t (&heightMap)[CHUNK_WIDTH][CHUNK_WIDTH]
) : ChunkPosition(chunkPosition),
    PerlinEngine(perlin),
    HeightMap(heightMap)
{
}

}
