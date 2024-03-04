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
    const siv::PerlinNoise::seed_type seed = 123456u;
    const siv::PerlinNoise perlin{seed};

    int32_t size = 4;
    for (int32_t i = -size; i <= size; ++i)
    {
        for (int32_t j = -size; j <= size; ++j)
        {
            MapPosition pos(glm::vec3(i, -1, j));
            Chunk chunk(pos.Vector, perlin);
            chunk.Generate();
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

    for (size_t x = 0; x < CHUNK_WIDTH; x++)
    {
        for (size_t y = 0; y < CHUNK_HEIGHT; y++)
        {
            Voxel &front = voxelGrid[x][CHUNK_WIDTH - 1][y];
            Voxel &back = voxelGrid[x][0][y];
            Voxel &right = voxelGrid[CHUNK_WIDTH - 1][x][y];
            Voxel &left = voxelGrid[0][x][y];

            if (zPositiveGrid != nullptr)
            {
                if ((*zPositiveGrid)[x][0].size() > y)
                    CheckVoxelEdge(front, (*zPositiveGrid)[x][0][y], VoxelFace::FRONT);
                else
                    front.SetFaceVisible(VoxelFace::FRONT, true);
            }
            if (zNegativeGrid != nullptr)
            {
                if ((*zNegativeGrid)[x][CHUNK_WIDTH - 1].size() > y)
                    CheckVoxelEdge(back, (*zNegativeGrid)[x][CHUNK_WIDTH - 1][y], VoxelFace::BACK);
                else
                    back.SetFaceVisible(VoxelFace::BACK, true);
            }
            if (xPositiveGrid != nullptr)
            {
                if ((*xPositiveGrid)[0][x].size() > y)
                    CheckVoxelEdge(right, (*xPositiveGrid)[0][x][y], VoxelFace::RIGHT);
                else
                    right.SetFaceVisible(VoxelFace::RIGHT, true);
            }
            if (xNegativeGrid != nullptr)
            {
                if ((*xNegativeGrid)[CHUNK_WIDTH - 1][x].size() > y)
                    CheckVoxelEdge(left, (*xNegativeGrid)[CHUNK_WIDTH - 1][x][y], VoxelFace::LEFT);
                else
                    left.SetFaceVisible(VoxelFace::LEFT, true);
            }
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
