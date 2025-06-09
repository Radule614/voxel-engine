#pragma once

#include <unordered_map>
#include <GLCoreUtils.hpp>
#include "../../Terrain/Position2D.hpp"

namespace VoxelEngine
{

struct ChunkRenderData
{
    GLuint VertexArray;
    GLuint VertexBuffer;
    GLuint IndexBuffer;
    std::vector<uint32_t> Indices;
    glm::mat4 ModelMatrix;
};

struct TerrainComponent
{
    std::unordered_map<Position2D, ChunkRenderData> RenderData;
};

}
