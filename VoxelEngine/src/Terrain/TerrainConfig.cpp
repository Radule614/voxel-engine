#include "TerrainConfig.hpp"
#include <glad/glad.h>

namespace VoxelEngine
{

int32_t TerrainConfig::ThreadCount = 2;
int32_t TerrainConfig::MaxChunkDistance = 6;

int32_t TerrainConfig::PolygonMode = GL_FILL;

}
