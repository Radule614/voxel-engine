#include "Config.hpp"

#include <glad/glad.h>

namespace VoxelEngine
{

int32_t Config::PolygonMode = GL_FILL;

int32_t Config::ChunkThreadCount = 4;
int32_t Config::MaxChunkDistance = 22;

uint32_t Config::ShadowHeight = 4096;
uint32_t Config::ShadowWidth = 4096;
float_t Config::ShadowFrustumSize = 120.0f;
float_t Config::ShadowNearPlane = 1.0f;
float_t Config::ShadowFarPlane = 100.0f;

uint32_t Config::PointShadowHeight = 1024;
uint32_t Config::PointShadowWidth = 1024;
float_t Config::PointShadowNearPlane = 0.5f;
float_t Config::PointShadowFarPlane = 30.0f;

}
