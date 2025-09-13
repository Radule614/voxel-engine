#pragma once

#include <vector>
#include <unordered_map>
#include <mutex>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "PerlinNoise.hpp"

#include "Voxel.hpp"
#include "VoxelMeshBuilder.hpp"
#include "Position2D.hpp"
#include "Position3D.hpp"
#include "Structure.hpp"
#include "TerrainConfig.hpp"
#include "VoxelVertex.hpp"

namespace VoxelEngine
{

class World;
using VoxelGrid = Voxel[CHUNK_WIDTH][CHUNK_WIDTH][CHUNK_HEIGHT];
using RadianceGrid = float_t[CHUNK_WIDTH + 2][CHUNK_WIDTH + 2][CHUNK_HEIGHT + 2];

class Chunk
{
public:
    Chunk(World& world, const siv::PerlinNoise& perlin);
    Chunk(World& world, Position2D position, const siv::PerlinNoise& perlin);
    ~Chunk();

    struct Neighbours
    {
        std::shared_ptr<Chunk> front = nullptr;
        std::shared_ptr<Chunk> back = nullptr;
        std::shared_ptr<Chunk> right = nullptr;
        std::shared_ptr<Chunk> left = nullptr;
    };

    void Generate();
    void GenerateMesh();
    void GenerateEdgeMesh(VoxelFace face);
    std::pair<Position2D, Position3D> GetPositionRelativeToWorld(glm::i32vec3 pos) const;

    VoxelGrid& GetVoxelGrid();
    RadianceGrid& GetRadianceGrid();
    const std::vector<VoxelVertex>& GetMesh() const;
    const std::vector<VoxelVertex>& GetBorderMesh(VoxelFace face) const;
    Position2D GetPosition() const;
    std::mutex& GetLock();
    glm::mat4 GetModelMatrix() const;

private:
    void DetermineEdgeMeshes(VoxelMeshBuilder& meshBuilder, Voxel& v, size_t x, size_t z);
    void AddEdgeMesh(VoxelMeshBuilder& meshBuilder, Voxel& v, VoxelFace f);
    void AddEdgeMesh(VoxelMeshBuilder& meshBuilder, Voxel& v, VoxelFace f1, VoxelFace f2);
    void DetermineVoxelFeatures(Voxel& v, size_t x, size_t z, size_t h);
    void AddStructures(std::vector<Structure> structures);
    void CalculateRadianceGrid();

private:
    World& m_World;
    Position2D m_Position;
    VoxelGrid m_VoxelGrid;
    std::vector<VoxelVertex> m_Mesh;
    std::unordered_map<VoxelFace, std::vector<VoxelVertex> > m_BorderMeshes;
    const siv::PerlinNoise& m_Perlin;
    std::mutex m_Mutex;

    RadianceGrid m_RadianceGrid;
};

};
