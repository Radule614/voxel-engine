//
// Created by RadU on 10/12/2025.
//

#include "ShrineGenerator.hpp"
#include "ShrineFactory.hpp"
#include "../../../World.hpp"

namespace VoxelEngine
{

class World;

void ShrineGenerator::Generate(const Context& context, std::vector<Structure>& output) const
{
    constexpr size_t regionSize = CHUNK_WIDTH / 4;

    std::vector<std::pair<Voxel, double_t> > possibleLocations{};
    for (size_t x = 0; x < CHUNK_WIDTH; x += regionSize)
    {
        for (size_t z = 0; z < CHUNK_WIDTH; z += regionSize)
        {
            const auto globalPosition = World::WorldToGlobalSpace(context.ChunkPosition, Position3D(x, 0, z));

            const double_t locationBias = context.Perlin.octave2D_01(((float_t) globalPosition.x - 2000.0f) * 0.01f,
                                                                     ((float_t) globalPosition.z - 2500.0f) * 0.01f,
                                                                     4);

            const int32_t rx = x + (int32_t) (locationBias * 452) % regionSize;
            const int32_t rz = z + (int32_t) (locationBias * 1927) % regionSize;

            const Voxel& surface = context.SurfaceLayer[rx][rz];

            possibleLocations.emplace_back(surface, locationBias);
        }
    }

    std::mt19937 rng(context.PerlinSeed);
    std::ranges::shuffle(possibleLocations, rng);

    if (!possibleLocations.empty())
    {
        auto& [voxel, bias] = possibleLocations[0];

        if (bias > 0.75)
            output.emplace_back(ShrineFactory::CreateShrine(voxel.GetPosition()));
    }
}

}
