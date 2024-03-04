#include "World.hpp"

#include <vector>
#include <GLCoreUtils.hpp>

namespace Terrain
{
World::World() : m_ChunkMap({})
{
}
World::~World()
{
}
const std::map<MapPosition, Chunk> &World::GetChunkMap() const
{
    return m_ChunkMap;
}
void World::Generate()
{
    int32_t size = 4;
    for (int32_t i = -size; i <= size; ++i)
    {
        for (int32_t j = -size; j <= size; ++j)
        {
            MapPosition pos(glm::vec3(i, -1, j));
            Chunk chunk(pos.Vector);
            CheckChunkEdges(chunk);
            m_ChunkMap.insert({pos, chunk});
        }
    }

    for (auto it = m_ChunkMap.begin(); it != m_ChunkMap.end(); ++it)
        it->second.GenerateMesh();
}

void World::CheckChunkEdges(Chunk &chunk)
{
    glm::vec3 pos = chunk.GetPosition();
    auto &voxelGrid = chunk.GetVoxelGrid();

    auto zPositive = m_ChunkMap.find(MapPosition(glm::vec3(pos.x, pos.y, pos.z + 1)));
    auto zNegative = m_ChunkMap.find(MapPosition(glm::vec3(pos.x, pos.y, pos.z - 1)));
    auto xPositive = m_ChunkMap.find(MapPosition(glm::vec3(pos.x + 1, pos.y, pos.z)));
    auto xNegative = m_ChunkMap.find(MapPosition(glm::vec3(pos.x - 1, pos.y, pos.z)));

    std::vector<std::vector<std::vector<Voxel>>> *zPositiveGrid = nullptr;
    std::vector<std::vector<std::vector<Voxel>>> *zNegativeGrid = nullptr;
    std::vector<std::vector<std::vector<Voxel>>> *xPositiveGrid = nullptr;
    std::vector<std::vector<std::vector<Voxel>>> *xNegativeGrid = nullptr;

    if (zPositive != m_ChunkMap.end())
        zPositiveGrid = &zPositive->second.GetVoxelGrid();
    if (zNegative != m_ChunkMap.end())
        zNegativeGrid = &zNegative->second.GetVoxelGrid();
    if (xPositive != m_ChunkMap.end())
        xPositiveGrid = &xPositive->second.GetVoxelGrid();
    if (xNegative != m_ChunkMap.end())
        xNegativeGrid = &xNegative->second.GetVoxelGrid();

    for (size_t y = 0; y < CHUNK_HEIGHT; y++)
    {
        for (size_t x = 0; x < CHUNK_WIDTH; x++)
        {
            Voxel &front = voxelGrid[y][x][CHUNK_WIDTH - 1];
            Voxel &back = voxelGrid[y][x][0];
            Voxel &right = voxelGrid[y][CHUNK_WIDTH - 1][x];
            Voxel &left = voxelGrid[y][0][x];

            if (zPositiveGrid != nullptr)
                CheckVoxelEdge(front, (*zPositiveGrid)[y][x][0], VoxelFace::FRONT);
            if (zNegativeGrid != nullptr)
                CheckVoxelEdge(back, (*zNegativeGrid)[y][x][CHUNK_WIDTH - 1], VoxelFace::BACK);
            if (xPositiveGrid != nullptr)
                CheckVoxelEdge(right, (*xPositiveGrid)[y][0][x], VoxelFace::RIGHT);
            if (xNegativeGrid != nullptr)
                CheckVoxelEdge(left, (*xNegativeGrid)[y][CHUNK_WIDTH - 1][x], VoxelFace::LEFT);
        }
    }
}

void World::CheckVoxelEdge(Voxel &v1, Voxel &v2, VoxelFace face)
{
    if (v1.GetVoxelType() != VoxelType::AIR && v2.GetVoxelType() == VoxelType::AIR)
        v1.SetFaceVisible(face, true);
    else if (v1.GetVoxelType() == VoxelType::AIR && v2.GetVoxelType() != VoxelType::AIR)
        v2.SetFaceVisible(Voxel::GetOpositeFace(face), true);
}
}; // namespace Terrain
