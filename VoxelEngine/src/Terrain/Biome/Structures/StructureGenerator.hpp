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
class Chunk;

class StructureGenerator
{
public:
    StructureGenerator();
    virtual ~StructureGenerator() = default;

    struct Context
    {
        Chunk& Chunk;
        const siv::PerlinNoise& Perlin;
        const int32_t PerlinSeed;
        const Voxel (&SurfaceLayer)[CHUNK_WIDTH][CHUNK_WIDTH];

        Context(VoxelEngine::Chunk& chunk,
                const siv::PerlinNoise& perlin,
                int32_t perlinSeed,
                const Voxel (&surfaceLayer)[CHUNK_WIDTH][CHUNK_WIDTH]);
    };

    virtual void Generate(const Context& context, std::vector<Structure>& output) const = 0;
};

}
