//
// Created by RadU on 9/21/2025.
//

#include "TreeGenerator.hpp"
#include "TreeFactory.hpp"
#include "../../../World.hpp"

namespace VoxelEngine
{

class World;

void TreeGenerator::Generate(const GenerationContext& context, std::vector<Structure>& output) const
{
    constexpr size_t offset = 2;
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

            const Voxel& voxel = context.SurfaceLayer[rx][rz];

            if (voxel.GetVoxelType() != GRASS)
                continue;

            possibleLocations.emplace_back(voxel, locationBias);
        }
    }

    std::mt19937 rng(context.PerlinSeed);
    std::ranges::shuffle(possibleLocations, rng);

    for (auto [voxel, locationBias]: possibleLocations)
    {
        const float_t chance = locationBias - output.size() * 0.12f;

        if (chance > 0.8f)
            output.emplace_back(TreeFactory::CreateLargeTree(voxel.GetPosition()));
        else if (chance > 0.6f)
            output.emplace_back(TreeFactory::CreateTree(voxel.GetPosition()));
    }
}

}
