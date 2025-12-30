#pragma once

#include <unordered_map>
#include "../../Terrain/Utils/Position2D.hpp"
#include "glad/glad.h"
#include "../Assets/Material.hpp"

namespace VoxelEngine
{

struct ChunkRenderData
{
    GLuint VertexArray;
    GLuint VertexBuffer;
    GLuint IndexBuffer;
    std::vector<uint32_t> Indices{};
    glm::mat4 ModelMatrix;
};

struct TerrainComponent
{
    Material TerrainMaterial;
    std::unordered_map<Position2D, ChunkRenderData> RenderData;

    explicit TerrainComponent(const Material& material) : TerrainMaterial(material)
    {
    }
};

}
