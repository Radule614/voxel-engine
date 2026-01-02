#include "Config.hpp"

#include <glad/glad.h>

namespace VoxelEngine
{

int32_t Config::PolygonMode = GL_FILL;

int32_t Config::ChunkThreadCount = 2;
int32_t Config::MaxChunkDistance = 10;

uint32_t Config::ShadowHeight = 1024;
uint32_t Config::ShadowWidth = 1024;
float_t Config::ShadowNearPlane = 0.5f;
float_t Config::ShadowFarPlane = 30.0f;

}
