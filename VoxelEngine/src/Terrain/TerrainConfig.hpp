#pragma once
#include <cstdint>

#define CHUNK_WIDTH 16
#define CHUNK_HEIGHT 128
#define RADIANCE_WIDTH 18
#define RADIANCE_HEIGHT 130

namespace VoxelEngine
{

struct TerrainConfig
{
    static int32_t ThreadCount;
    static int32_t PolygonMode;
};

}
