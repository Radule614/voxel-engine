#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Vertex.hpp"
#include "Voxel.hpp"

#define CHUNK_WIDTH 16
#define CHUNK_HEIGHT 64

namespace Terrain
{
class Chunk
{
public:
    Chunk();
    Chunk(glm::vec3 position);
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

    void GenerateMesh();

private:
    void Init();

private:
    glm::vec3 m_Position;
    std::vector<Vertex> m_Mesh;
    std::vector<std::vector<std::vector<Voxel>>> m_VoxelGrid;
};
}; // namespace Terrain
