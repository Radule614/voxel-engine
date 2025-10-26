//
// Created by RadU on 10/26/2025.
//

#include "LampGenerator.hpp"
#include "LampFactory.hpp"
#include "Terrain/World/World.hpp";

using namespace VoxelEngine;

namespace Sandbox
{

void LampGenerator::Generate(const Context& context, std::vector<Structure>& output) const
{
    constexpr size_t regionSize = CHUNK_WIDTH;

    const auto globalPosition = World::WorldToGlobalSpace(context.ChunkPosition, Position3D(7, 0, 7));

    const double_t locationBias = context.Perlin.octave2D_01(((float_t) globalPosition.x - 5000.0f) * 0.01f,
                                                             ((float_t) globalPosition.z - 500.0f) * 0.01f,
                                                             4);

    const int32_t rx = (int32_t) (locationBias * 7181) % regionSize;
    const int32_t rz = (int32_t) (locationBias * 5931) % regionSize;

    const Voxel& surface = context.SurfaceLayer[rx][rz];

    if (surface.GetVoxelType() != AIR && locationBias > 0.7)
        output.emplace_back(LampFactory::CreateLamp(surface.GetPosition()));
}

}
