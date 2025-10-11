//
// Created by RadU on 9/27/2025.
//

#include "Biome.hpp"
#include "../TerrainConfig.hpp"
#include "Structures/Tree/TreeGenerator.hpp"

namespace VoxelEngine
{

Biome::Biome(const uint32_t seed) : m_Perlin(seed), m_PerlinSeed(seed), m_Generator(std::make_unique<TreeGenerator>())
{
}

VoxelType Biome::ResolveVoxelType(const glm::i32vec3 globalPosition, const int32_t height) const
{
    const double_t density = GetDensity(globalPosition, height);

    VoxelType type = AIR;
    if (density >= 0)
    {
        type = STONE;

        const double_t sandMask = m_Perlin.octave2D_01((globalPosition.x + 2000.0) * 0.001f,
                                                       (globalPosition.z + 3000.0) * 0.001f,
                                                       8);

        if (globalPosition.y > height - 5 && sandMask >= 0.62) { type = SAND; }
        else
        {
            if (globalPosition.y > height - 5)
                type = DIRT;
            if (globalPosition.y == height - 1)
                type = GRASS;
        }
    }
    if (globalPosition.y == 0)
        type = STONE;

    return type;
}

double_t Biome::GetDensity(const glm::i32vec3 globalPosition, const int32_t height) const
{
    const glm::vec3 vec = glm::vec3(globalPosition) * 0.02f;

    double_t density = m_Perlin.octave3D(vec.x, vec.y, vec.z, 4);
    density += 1 - static_cast<double_t>(globalPosition.y + height / 4) / CHUNK_HEIGHT;

    return density;
}

int32_t Biome::GetHeight(const int32_t x, const int32_t z) const
{
    const double_t heightMask = m_Perlin.octave2D_01(x * 0.005f, z * 0.005f, 8);
    const double_t baseHeight = CHUNK_HEIGHT / 5 + heightMask * 3 * CHUNK_HEIGHT / 10;

    const double_t mountainMask = m_Perlin.octave2D_01((x + 1000.0) * 0.007f, (z + 1000.0) * 0.007f, 8);
    const double_t mountainHeight = glm::max(0.0, mountainMask - 0.5f) * (7 * CHUNK_HEIGHT / 10);

    const double_t finalHeight = glm::floor(baseHeight + mountainHeight);

    return (int32_t) finalHeight;
}

void Biome::GenerateStructures(const Voxel (&surfaceLayer)[CHUNK_WIDTH][CHUNK_WIDTH],
                               const Position2D chunkPosition,
                               std::vector<Structure>& output) const
{
    const StructureGenerator::GenerationContext context(m_Perlin, m_PerlinSeed, surfaceLayer, chunkPosition);

    m_Generator->Generate(context, output);
}

}
