//
// Created by RadU on 10/11/2025.
//

#include "CactusGenerator.hpp"
#include "CactusFactory.hpp"
#include "../../../World.hpp"

namespace VoxelEngine
{

class World;

void CactusGenerator::Generate(const Context& context, std::vector<Structure>& output) const
{
    constexpr size_t offset = 0;
    constexpr size_t regionSize = (CHUNK_WIDTH - 2 * offset) / 4;

    std::vector<std::pair<Voxel, double_t> > possibleLocations{};
    for (size_t x = offset; x < CHUNK_WIDTH - offset; x += regionSize)
    {
        for (size_t z = offset; z < CHUNK_WIDTH - offset; z += regionSize)
        {
            const auto globalPosition = World::WorldToGlobalSpace(context.ChunkPosition, Position3D(x, 0, z));

            const double_t locationBias = context.Perlin.octave2D_01(((float_t) globalPosition.x - 2000.0f) * 0.01f,
                                                                     ((float_t) globalPosition.z - 2500.0f) * 0.01f,
                                                                     4);

            const int32_t rx = x + (int32_t) (locationBias * 7773) % regionSize;
            const int32_t rz = z + (int32_t) (locationBias * 4351) % regionSize;

            const Voxel& voxel = context.SurfaceLayer[rx][rz];

            if (voxel.GetVoxelType() != SAND)
                continue;

            possibleLocations.emplace_back(voxel, locationBias);
        }
    }

    std::mt19937 rng(context.PerlinSeed);
    std::ranges::shuffle(possibleLocations, rng);

    for (const auto& [voxel, locationBias]: possibleLocations)
    {
        const float_t chance = locationBias - output.size() * 0.06f;

        if (chance > 0.6f)
            output.emplace_back(CactusFactory::CreateCactus(voxel.GetPosition()));
    }
}

}
