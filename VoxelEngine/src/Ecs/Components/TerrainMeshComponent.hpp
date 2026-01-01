//
// Created by RadU on 1/1/2026.
//

#pragma once

#include <vector>
#include "glad/glad.h"
#include "../../Terrain/Utils/Position2D.hpp"
#include "../../Assets/Material.hpp"
#include "../../Config.hpp"

namespace VoxelEngine
{

struct TerrainMeshComponent
{
    Position2D Position;
    Material Material;

    GLuint VertexArray = 0;
    GLuint VertexBuffer = 0;
    GLuint IndexBuffer = 0;
    std::vector<uint32_t> Indices{};

    TerrainMeshComponent(const Position2D Position, const VoxelEngine::Material& material)
        : Position(Position), Material(material)
    {
    }

    glm::mat4 GetModelMatrix() const
    {
        auto pos = glm::vec3(Position.x, 0, Position.y);

        pos.x *= CHUNK_WIDTH;
        pos.y *= CHUNK_HEIGHT;
        pos.z *= CHUNK_WIDTH;

        return glm::translate(glm::mat4(1.0f), pos);
    }
};

}
