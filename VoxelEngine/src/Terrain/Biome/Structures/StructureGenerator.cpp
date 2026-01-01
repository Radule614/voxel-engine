//
// Created by RadU on 9/23/2025.
//

#include "StructureGenerator.hpp"

namespace VoxelEngine
{

StructureGenerator::StructureGenerator() = default;

StructureGenerator::Context::Context(
    VoxelEngine::Chunk& chunk,
    const siv::PerlinNoise& perlin,
    const int32_t perlinSeed,
    const Voxel (&surfaceLayer)[CHUNK_WIDTH][CHUNK_WIDTH]
) : Chunk(chunk), Perlin(perlin), PerlinSeed(perlinSeed), SurfaceLayer(surfaceLayer)
{
}

}
