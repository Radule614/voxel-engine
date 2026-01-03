#pragma once

#include <cmath>
#include <cstdint>

#define CHUNK_WIDTH 16
#define CHUNK_HEIGHT 144

namespace VoxelEngine
{

struct Config
{
    static int32_t PolygonMode;

    static int32_t ChunkThreadCount;
    static int32_t MaxChunkDistance;

    static uint32_t ShadowHeight;
    static uint32_t ShadowWidth;
    static float_t ShadowFrustumSize;
    static float_t ShadowNearPlane;
    static float_t ShadowFarPlane;

    static uint32_t PointShadowHeight;
    static uint32_t PointShadowWidth;
    static float_t PointShadowNearPlane;
    static float_t PointShadowFarPlane;
};

}
