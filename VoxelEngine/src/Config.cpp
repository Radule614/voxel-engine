#include "Config.hpp"

#include <glad/glad.h>

namespace VoxelEngine
{

int32_t Config::PolygonMode = GL_FILL;

int32_t Config::ChunkThreadCount = 2;
int32_t Config::MaxChunkDistance = 5;

uint32_t Config::ShadowHeight = 2048;
uint32_t Config::ShadowWidth = 2048;
float_t Config::ShadowNearPlane = 1.0f;
float_t Config::ShadowFarPlane = 30.0f;

}
