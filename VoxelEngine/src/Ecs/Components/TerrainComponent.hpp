#pragma once

#include <unordered_map>
#include "../../Terrain/Utils/Position2D.hpp"

namespace VoxelEngine
{

struct ChunkRenderData
{
    GLuint VertexArray;
    GLuint VertexBuffer;
    GLuint IndexBuffer;
    GLuint RadianceStorageBuffer;
    std::vector<uint32_t> Indices{};
    glm::mat4 ModelMatrix;
};

struct TerrainComponent
{
    std::unordered_map<Position2D, ChunkRenderData> RenderData;
};

}
