#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "PerlinNoise.hpp"

#include "Vertex.hpp"
#include "Voxel.hpp"

#define CHUNK_WIDTH 24
#define CHUNK_HEIGHT 32

namespace Terrain
{
class Chunk
{
public:
    Chunk(const siv::PerlinNoise &perlin);
    Chunk(glm::vec3 position, const siv::PerlinNoise &perlin);
    ~Chunk();

    inline std::vector<Vertex> GetMesh() const
    {
        return m_Mesh;
    }

    inline glm::vec3 GetPosition() const
    {
        return m_Position;
    }

    glm::mat4 GetModelMatrix() const;

    std::vector<std::vector<std::vector<Voxel>>> &GetVoxelGrid()
    {
        return m_VoxelGrid;
    }

    void Generate();
    void GenerateMesh();

private:
    glm::vec3 m_Position;
    std::vector<Vertex> m_Mesh;
    std::vector<std::vector<std::vector<Voxel>>> m_VoxelGrid;
    const siv::PerlinNoise &m_Perlin;
};
}; // namespace Terrain
