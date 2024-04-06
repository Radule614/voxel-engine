#pragma once

#include <vector>
#include <unordered_map>
#include <mutex>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "PerlinNoise.hpp"

#include "Vertex.hpp"
#include "Voxel.hpp"
#include "VoxelMeshBuilder.hpp"

#define CHUNK_WIDTH 16
#define CHUNK_HEIGHT 48
#define THREADS 2

namespace Terrain
{
class Chunk
{
public:
    Chunk(const siv::PerlinNoise &perlin);
    Chunk(glm::vec2 position, const siv::PerlinNoise &perlin);
    ~Chunk();

    struct Neighbours
    {
        std::shared_ptr<Chunk> front = nullptr;
        std::shared_ptr<Chunk> back = nullptr;
        std::shared_ptr<Chunk> right = nullptr;
        std::shared_ptr<Chunk> left = nullptr;
    };

    inline const std::vector<Vertex> &GetMesh() const
    {
        return m_Mesh;
    }
    inline const std::vector<Vertex> &GetBorderMesh(VoxelFace face) const
    {
        return m_BorderMeshes.at(face);
    }

    inline glm::vec2 GetPosition() const
    {
        return m_Position;
    }

    std::mutex &GetLock()
    {
        return m_Mutex;
    }

    glm::mat4 GetModelMatrix() const;

    void Generate();
    void GenerateMesh();
    void GenerateEdgeMesh(VoxelFace face);

private:
    void DetermineEdgeMeshes(VoxelMeshBuilder &meshBuilder, Voxel &v, size_t x, size_t z);
    void AddEdgeMesh(VoxelMeshBuilder &meshBuilder, Voxel &v, VoxelFace f);
    void AddEdgeMesh(VoxelMeshBuilder &meshBuilder, Voxel &v, VoxelFace f1, VoxelFace f2);
    void DetermineVoxelFeatures(Voxel &v, size_t x, size_t z, size_t h);

public:
    Voxel m_VoxelGrid[CHUNK_WIDTH][CHUNK_WIDTH][CHUNK_HEIGHT];

private:
    glm::vec2 m_Position;
    std::vector<Vertex> m_Mesh;
    std::unordered_map<VoxelFace, std::vector<Vertex>> m_BorderMeshes;
    const siv::PerlinNoise &m_Perlin;
    std::mutex m_Mutex;
};
}; // namespace Terrain
