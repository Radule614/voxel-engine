//
// Created by RadU on 9/23/2025.
//

#include "StructureGenerator.hpp"

namespace VoxelEngine
{

StructureGenerator::StructureGenerator() = default;

StructureGenerator::GenerationContext::GenerationContext(
    const siv::PerlinNoise& perlin,
    const Voxel (&surfaceLayer)[CHUNK_WIDTH][CHUNK_WIDTH],
    const Position2D chunkPosition
) : Perlin(perlin), SurfaceLayer(surfaceLayer), ChunkPosition(chunkPosition)
{
}

}
