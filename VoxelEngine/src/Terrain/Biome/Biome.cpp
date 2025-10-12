//
// Created by RadU on 9/27/2025.
//

#include "Biome.hpp"
#include "../TerrainConfig.hpp"
#include "Structures/Cactus/CactusGenerator.hpp"
#include "Structures/DarkTree/DarkTreeGenerator.hpp"
#include "Structures/Tree/TreeGenerator.hpp"

namespace VoxelEngine
{

Biome::Biome(const uint32_t seed) : m_Perlin(seed),
                                    m_PerlinSeed(seed)
{
    m_Generators[PLAINS] = std::make_unique<TreeGenerator>();
    m_Generators[DESERT] = std::make_unique<CactusGenerator>();
    m_Generators[SNOWY_PLAINS] = std::make_unique<DarkTreeGenerator>();
}

Biome::GeneratorContext::GeneratorContext(const Voxel (&surfaceLayer)[16][16],
                                          const Position2D chunkPosition,
                                          const std::set<BiomeType>& chunkBiomeTypes)
    : SurfaceLayer(surfaceLayer),
      ChunkPosition(chunkPosition),
      ChunkBiomeTypes(chunkBiomeTypes)
{
}

std::tuple<BiomeType, VoxelType> Biome::ResolveBiomeFeatures(const glm::i32vec3 position, const int32_t height) const
{
    BiomeType biomeType = ResolveBiomeType(position.x, position.z);
    VoxelType voxelType = AIR;

    const double_t density = GetDensity(position, height);
    if (density >= 0)
    {
        voxelType = STONE;

        if (biomeType == DESERT && position.y > height - 6)
            voxelType = SAND;

        if (biomeType == PLAINS || biomeType == SNOWY_PLAINS)
        {
            if (position.y > height - 5)
                voxelType = DIRT;

            if (position.y == height - 1)
            {
                if (biomeType == PLAINS)
                    voxelType = DIRT_GRASS;

                if (biomeType == SNOWY_PLAINS)
                    voxelType = DIRT_SNOW;
            }
        }
    }

    if (position.y == 0)
        voxelType = STONE;

    return {biomeType, voxelType};
}

double_t Biome::GetDensity(const glm::i32vec3 position, const int32_t height) const
{
    const glm::vec3 vec = glm::vec3(position) * 0.02f;

    double_t density = m_Perlin.octave3D(vec.x, vec.y, vec.z, 4);
    density += 1 - static_cast<double_t>(position.y + height / 4) / CHUNK_HEIGHT;

    return density;
}

BiomeType Biome::ResolveBiomeType(const int32_t x, const int32_t z) const
{
    const double_t temperatureBias = m_Perlin.octave2D_01((x + 2000.0) * 0.001f, (z + 3000.0) * 0.001f, 8);

    if (temperatureBias < 0.3)
        return SNOWY_PLAINS;

    if (temperatureBias > 0.7)
        return DESERT;

    return PLAINS;
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

void Biome::GenerateStructures(const GeneratorContext& ctx, std::vector<Structure>& output) const
{
    const StructureGenerator::Context generatorContext(m_Perlin, m_PerlinSeed, ctx.SurfaceLayer, ctx.ChunkPosition);

    for (auto type: ctx.ChunkBiomeTypes)
    {
        if (auto generator = m_Generators.find(type); generator != m_Generators.end())
            generator->second->Generate(generatorContext, output);
    }
}

}
