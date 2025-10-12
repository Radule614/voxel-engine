//
// Created by RadU on 10/12/2025.
//

#include "DarkTreeGenerator.hpp"
#include "DarkTreeFactory.hpp"
#include "../../../World.hpp"

namespace VoxelEngine
{

class World;

void DarkTreeGenerator::Generate(const Context& context, std::vector<Structure>& output) const
{
    constexpr size_t offset = 0;
    constexpr size_t regionSize = (CHUNK_WIDTH - 2 * offset) / 2;

    std::vector<std::pair<Voxel, double_t> > possibleLocations{};
    for (size_t x = offset; x < CHUNK_WIDTH - offset; x += regionSize)
    {
        for (size_t z = offset; z < CHUNK_WIDTH - offset; z += regionSize)
        {
            const auto globalPosition = World::WorldToGlobalSpace(context.ChunkPosition, Position3D(x, 0, z));

            const double_t locationBias = context.Perlin.octave2D_01(((float_t) globalPosition.x + 3000.0f) * 0.01f,
                                                                     ((float_t) globalPosition.z + 4000.0f) * 0.01f,
                                                                     4);

            const int32_t rx = x + (int32_t) (locationBias * 1373) % regionSize;
            const int32_t rz = z + (int32_t) (locationBias * 10113) % regionSize;

            const Voxel& surface = context.SurfaceLayer[rx][rz];

            if (surface.GetVoxelType() != DIRT_SNOW)
                continue;

            possibleLocations.emplace_back(surface, locationBias);
        }
    }

    std::mt19937 rng(context.PerlinSeed);
    std::ranges::shuffle(possibleLocations, rng);

    for (const auto& [voxel, locationBias]: possibleLocations)
    {
        const float_t chance = locationBias - output.size() * 0.12f;

        if (chance > 0.73f)
            output.emplace_back(DarkTreeFactory::CreateLargeTree(voxel.GetPosition()));
        else if (chance > 0.6f)
            output.emplace_back(DarkTreeFactory::CreateTree(voxel.GetPosition()));
    }
}

}
