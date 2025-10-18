#pragma once
#include <cstdint>

#define CHUNK_WIDTH 16
#define CHUNK_HEIGHT 144
#define RADIANCE_WIDTH (CHUNK_WIDTH + 2)
#define RADIANCE_HEIGHT (CHUNK_HEIGHT + 2)

namespace VoxelEngine
{

struct TerrainConfig
{
    static int32_t ThreadCount;
    static int32_t MaxChunkDistance;
    static int32_t PolygonMode;
    static int32_t MaxRadiance;
    static int32_t SunRadiance;
};

}
