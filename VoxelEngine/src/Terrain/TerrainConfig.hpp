#pragma once
#include <cstdint>
#include <glad/glad.h>

#define CHUNK_WIDTH 16
#define CHUNK_HEIGHT 128

namespace VoxelEngine
{

struct TerrainConfig
{
	static int32_t ThreadCount;
	static int32_t PolygonMode;
};

}
