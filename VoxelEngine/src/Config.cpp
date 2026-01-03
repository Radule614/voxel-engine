#include "Config.hpp"

#include <glad/glad.h>

namespace VoxelEngine
{

int32_t Config::PolygonMode = GL_FILL;

int32_t Config::ChunkThreadCount = 2;
int32_t Config::MaxChunkDistance = 12;

uint32_t Config::ShadowHeight = 4096;
uint32_t Config::ShadowWidth = 4096;
float_t Config::ShadowNearPlane = 0.5f;
float_t Config::ShadowFarPlane = 30.0f;

}
