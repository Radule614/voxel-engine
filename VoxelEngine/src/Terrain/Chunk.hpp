#pragma once

#include <vector>
#include <unordered_map>
#include <mutex>
#include <queue>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "PerlinNoise.hpp"

#include "Voxel.hpp"
#include "VoxelMeshBuilder.hpp"
#include "Position2D.hpp"
#include "Position3D.hpp"
#include "Structures/Structure.hpp"
#include "TerrainConfig.hpp"
#include "VoxelVertex.hpp"
#include "Biome/Biome.hpp"
#include "Generators/StructureGenerator.hpp"

namespace VoxelEngine
{

class World;
using VoxelGrid = Voxel[CHUNK_WIDTH][CHUNK_WIDTH][CHUNK_HEIGHT];
using RadianceArray = int32_t[RADIANCE_WIDTH * RADIANCE_WIDTH * RADIANCE_HEIGHT];

class Chunk
{
public:
    Chunk(World& world,
          const Biome& biome,
          const std::vector<std::unique_ptr<StructureGenerator> >& generators);
    Chunk(World& world,
          Position2D position,
          const Biome& biome,
          const std::vector<std::unique_ptr<StructureGenerator> >& generators);
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
    std::pair<Position2D, Position3D> GetPositionRelativeToWorld(glm::ivec3 pos) const;

    VoxelGrid& GetVoxelGrid();
    RadianceArray& GetRadianceGrid();
    const std::vector<VoxelVertex>& GetMesh() const;
    const std::vector<VoxelVertex>& GetBorderMesh(VoxelFace face) const;
    Position2D GetPosition() const;
    std::mutex& GetLock();
    glm::mat4 GetModelMatrix() const;

    int32_t GetRadiance(size_t x, size_t z, size_t y) const;
    void UpdateRadiance(size_t x, size_t z, size_t y, int32_t radiance);
    void CommitRadianceChanges();

private:
    void DetermineEdgeMeshes(VoxelMeshBuilder& meshBuilder, Voxel& v, size_t x, size_t z);
    void AddEdgeMesh(VoxelMeshBuilder& meshBuilder, Voxel& v, VoxelFace f);
    void AddEdgeMesh(VoxelMeshBuilder& meshBuilder, Voxel& v, VoxelFace f1, VoxelFace f2);
    void DetermineVoxelFeatures(Voxel& v, size_t x, size_t z, size_t h) const;
    void AddStructures(const std::vector<Structure>& structures);

    void InitRadiance();
    void SetRadiance(size_t x, size_t z, size_t y, int32_t radiance);

private:
    World& m_World;
    Position2D m_Position;
    VoxelGrid m_VoxelGrid;
    std::vector<VoxelVertex> m_Mesh;
    std::unordered_map<VoxelFace, std::vector<VoxelVertex> > m_BorderMeshes;
    std::mutex m_Mutex;

    const Biome& m_Biome;
    const std::vector<std::unique_ptr<StructureGenerator> >& m_Generators;

    RadianceArray m_RadianceGrid;
    std::queue<glm::ivec3> m_RadianceUpdateQueue;
};

};
