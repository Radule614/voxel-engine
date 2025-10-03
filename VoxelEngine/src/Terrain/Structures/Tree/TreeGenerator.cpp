//
// Created by RadU on 9/21/2025.
//

#include "TreeGenerator.hpp"
#include "TreeFactory.hpp"
#include "../../World.hpp"

namespace VoxelEngine
{

class World;

void TreeGenerator::Generate(const GenerationContext& context, std::vector<Structure>& output) const
{
    constexpr size_t regionSize = 16;

    for (size_t x = 0; x < CHUNK_WIDTH; x += regionSize)
    {
        for (size_t z = 0; z < CHUNK_WIDTH; z += regionSize)
        {
            const auto globalPosition = World::WorldToGlobalSpace(context.ChunkPosition, Position3D(x, 0, z));

            const double_t treeBias = context.Perlin.octave2D_01((float_t) globalPosition.x * 0.001f,
                                                                 (float_t) globalPosition.z * 0.001f,
                                                                 4);

            const int32_t rx = x + (int32_t) (treeBias * 17153) % regionSize;
            const int32_t rz = z + (int32_t) (treeBias * 53751) % regionSize;

            const Voxel& voxel = context.SurfaceLayer[rx][rz];

            if (voxel.GetVoxelType() != GRASS)
                continue;

            if (treeBias > 0.8f)
                output.emplace_back(TreeFactory::CreateLargeTree(voxel.GetPosition()));
            else if (treeBias > 0.6f)
                output.emplace_back(TreeFactory::CreateTree(voxel.GetPosition()));
        }
    }
}

}
