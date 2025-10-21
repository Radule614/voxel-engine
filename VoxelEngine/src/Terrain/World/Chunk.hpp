#pragma once

#include <vector>
#include <unordered_map>
#include <mutex>
#include <queue>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "PerlinNoise.hpp"

#include "../Voxel/Voxel.hpp"
#include "../Voxel/VoxelMeshBuilder.hpp"
#include "../Utils/Position2D.hpp"
#include "../Biome/Structures/Structure.hpp"
#include "../TerrainConfig.hpp"
#include "../Biome/Biome.hpp"

namespace VoxelEngine
{

class World;

using VoxelGrid = Voxel[CHUNK_WIDTH][CHUNK_WIDTH][CHUNK_HEIGHT];
using RadianceArray = int32_t[RADIANCE_WIDTH * RADIANCE_WIDTH * RADIANCE_HEIGHT];

class Chunk
{
public:
    Chunk(World& world, const Biome& biome);
    Chunk(World& world, Position2D position, const Biome& biome);
    ~Chunk();
    Chunk(const Chunk&) = delete;
    Chunk& operator=(const Chunk&) = delete;

    void Generate();
    void GenerateMesh();
    void GenerateEdgeMesh(VoxelFace face);
    std::pair<Position2D, Position3D> GetPositionRelativeToWorld(glm::ivec3 pos) const;

    VoxelGrid& GetVoxelGrid();
    const RadianceArray& GetRadianceGrid() const;
    Voxel& GetVoxelFromGrid(Position3D positionInGrid);

    const std::vector<VoxelVertex>& GetMesh() const;
    const std::vector<VoxelVertex>& GetBorderMesh(VoxelFace face) const;
    Position2D GetPosition() const;
    std::mutex& GetLock();
    glm::mat4 GetModelMatrix() const;

    void InitRadiance();
    int32_t GetRadiance(size_t x, size_t z, size_t y) const;
    void UpdateRadiance(size_t x, size_t z, size_t y, int32_t radiance);
    void CommitRadianceChanges();

private:
    void DetermineEdgeMeshes(VoxelMeshBuilder& meshBuilder, Voxel& v, size_t x, size_t z);
    void AddEdgeMesh(VoxelMeshBuilder& meshBuilder, Voxel& v, VoxelFace f);
    void AddEdgeMesh(VoxelMeshBuilder& meshBuilder, Voxel& v, VoxelFace f1, VoxelFace f2);
    void DetermineVoxelFeatures(Voxel& v, size_t x, size_t z, int32_t h);
    void AddStructures(const std::vector<Structure>& structures);

    void SetRadiance(size_t x, size_t z, size_t y, int32_t radiance);

private:
    World& m_World;
    Position2D m_Position;
    VoxelGrid m_VoxelGrid;
    std::vector<VoxelVertex> m_Mesh;
    std::unordered_map<VoxelFace, std::vector<VoxelVertex> > m_BorderMeshes;
    std::mutex m_Lock;

    const Biome& m_Biome;

    RadianceArray m_RadianceGrid;
    std::queue<glm::ivec3> m_RadianceUpdateQueue;

    std::unordered_set<BiomeType> m_BiomeTypes;
    std::mutex m_BiomeLock;
};

}
