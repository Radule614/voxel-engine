#pragma once
#include <cstdint>

#define CHUNK_WIDTH 16
#define CHUNK_HEIGHT 144

namespace VoxelEngine
{

struct TerrainConfig
{
    static int32_t ThreadCount;
    static int32_t MaxChunkDistance;

    static int32_t PolygonMode;
};

}
