#pragma once

#include <vector>
#include <unordered_map>
#include <mutex>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "entt.hpp"
#include "../Voxel/Voxel.hpp"
#include "../Voxel/VoxelMeshBuilder.hpp"
#include "../Utils/Position2D.hpp"
#include "../Biome/Structures/Structure.hpp"
#include "../../Config.hpp"
#include "../Biome/Biome.hpp"

namespace VoxelEngine
{

class World;

using VoxelGrid = Voxel[CHUNK_WIDTH][CHUNK_WIDTH][CHUNK_HEIGHT];

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
    Voxel& GetVoxelFromGrid(Position3D positionInGrid);
    void AddPointLight(glm::vec3 position, glm::vec3 color = glm::vec3(1.0f));

    const std::vector<VoxelVertex>& GetMesh() const;
    const std::vector<VoxelVertex>& GetBorderMesh(VoxelFace face) const;
    Position2D GetPosition() const;
    glm::ivec3 GetWorldPosition() const;
    std::mutex& GetLock();
    glm::mat4 GetModelMatrix() const;

private:
    void DetermineEdgeMeshes(VoxelMeshBuilder& meshBuilder, Voxel& v, size_t x, size_t z);
    void AddEdgeMesh(VoxelMeshBuilder& meshBuilder, Voxel& v, VoxelFace f);
    void AddEdgeMesh(VoxelMeshBuilder& meshBuilder, Voxel& v, VoxelFace f1, VoxelFace f2);
    void DetermineVoxelFeatures(Voxel& v, size_t x, size_t z, int32_t h);
    void AddStructures(const std::vector<Structure>& structures);

private:
    World& m_World;
    Position2D m_Position;
    VoxelGrid m_VoxelGrid;
    std::vector<VoxelVertex> m_Mesh;
    std::unordered_map<VoxelFace, std::vector<VoxelVertex> > m_BorderMeshes;
    std::mutex m_Lock;

    const Biome& m_Biome;

    std::unordered_set<BiomeType> m_BiomeTypes;
    std::mutex m_BiomeLock;

    std::vector<entt::entity> m_Entities;
};

}
