#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "PerlinNoise.hpp"

#include "Vertex.hpp"
#include "Voxel.hpp"

#define CHUNK_WIDTH 24
#define CHUNK_HEIGHT 48

namespace Terrain
{
class Chunk
{
public:
    Chunk(const siv::PerlinNoise &perlin);
    Chunk(glm::vec3 position, const siv::PerlinNoise &perlin);
    ~Chunk();

    struct Neighbours
    {
        std::shared_ptr<Chunk> front = nullptr;
        std::shared_ptr<Chunk> back = nullptr;
        std::shared_ptr<Chunk> right = nullptr;
        std::shared_ptr<Chunk> left = nullptr;
    };

    inline std::vector<Vertex> GetMesh() const
    {
        return m_Mesh;
    }

    inline glm::vec3 GetPosition() const
    {
        return m_Position;
    }

    glm::mat4 GetModelMatrix() const;

    void Generate();
    void GenerateMesh();

public:
    Voxel m_VoxelGrid[CHUNK_WIDTH][CHUNK_WIDTH][CHUNK_HEIGHT];

private:
    glm::vec3 m_Position;
    std::vector<Vertex> m_Mesh;
    const siv::PerlinNoise &m_Perlin;
};
}; // namespace Terrain
