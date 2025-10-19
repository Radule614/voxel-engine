#include "TerrainConfig.hpp"
#include <glad/glad.h>

namespace VoxelEngine
{

int32_t TerrainConfig::ThreadCount = 4;
int32_t TerrainConfig::MaxChunkDistance = 20;

int32_t TerrainConfig::PolygonMode = GL_FILL;

int32_t TerrainConfig::MaxRadiance = 10;
int32_t TerrainConfig::SunRadiance = 10;

}
