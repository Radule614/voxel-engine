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
    std::vector<std::vector<std::vector<Voxel>>> &voxelGrid = chunk.GetVoxelGrid();
    auto zPositive = m_ChunkMap.find(MapPosition(glm::vec3(pos.x, pos.y, pos.z + 1)));
    auto zNegative = m_ChunkMap.find(MapPosition(glm::vec3(pos.x, pos.y, pos.z - 1)));
    auto xPositive = m_ChunkMap.find(MapPosition(glm::vec3(pos.x + 1, pos.y, pos.z)));
    auto xNegative = m_ChunkMap.find(MapPosition(glm::vec3(pos.x - 1, pos.y, pos.z)));

    for (size_t x = 0; x < CHUNK_WIDTH; x++)
    {
        for (size_t y = 0; y < CHUNK_HEIGHT; y++)
        {
            Voxel &front = voxelGrid[x][y][CHUNK_WIDTH - 1];
            Voxel &back = voxelGrid[x][y][0];
            Voxel &right = voxelGrid[CHUNK_WIDTH - 1][y][x];
            Voxel &left = voxelGrid[0][y][x];

            if (zPositive != m_ChunkMap.end())
            {
                auto &zPositiveVoxelGrid = zPositive->second.GetVoxelGrid();
                Voxel &zPositiveBack = zPositiveVoxelGrid[x][y][0];
                if (front.GetVoxelType() != VoxelType::AIR && zPositiveBack.GetVoxelType() == VoxelType::AIR)
                    front.SetFaceVisible(VoxelFace::FRONT, true);
                else if (front.GetVoxelType() == VoxelType::AIR && zPositiveBack.GetVoxelType() != VoxelType::AIR)
                    zPositiveBack.SetFaceVisible(VoxelFace::BACK, true);
            }

            if (zNegative != m_ChunkMap.end())
            {
                auto &zNegativeVoxelGrid = zNegative->second.GetVoxelGrid();
                Voxel &zNegativeFront = zNegativeVoxelGrid[x][y][CHUNK_WIDTH - 1];
                if (back.GetVoxelType() != VoxelType::AIR && zNegativeFront.GetVoxelType() == VoxelType::AIR)
                    back.SetFaceVisible(VoxelFace::BACK, true);
                if (back.GetVoxelType() == VoxelType::AIR && zNegativeFront.GetVoxelType() != VoxelType::AIR)
                    zNegativeFront.SetFaceVisible(VoxelFace::FRONT, true);
            }

            if (xPositive != m_ChunkMap.end())
            {
                auto &xPositiveVoxelGrid = xPositive->second.GetVoxelGrid();
                Voxel &xPositiveLeft = xPositiveVoxelGrid[0][y][x];
                if (right.GetVoxelType() != VoxelType::AIR && xPositiveLeft.GetVoxelType() == VoxelType::AIR)
                    right.SetFaceVisible(VoxelFace::RIGHT, true);
                else if (right.GetVoxelType() == VoxelType::AIR && xPositiveLeft.GetVoxelType() != VoxelType::AIR)
                    xPositiveLeft.SetFaceVisible(VoxelFace::LEFT, true);
            }

            if (xNegative != m_ChunkMap.end())
            {
                auto &xNegativeVoxelGrid = xNegative->second.GetVoxelGrid();
                Voxel &xNegativeFront = xNegativeVoxelGrid[CHUNK_WIDTH - 1][y][x];
                if (left.GetVoxelType() != VoxelType::AIR && xNegativeFront.GetVoxelType() == VoxelType::AIR)
                    left.SetFaceVisible(VoxelFace::LEFT, true);
                if (left.GetVoxelType() == VoxelType::AIR && xNegativeFront.GetVoxelType() != VoxelType::AIR)
                    xNegativeFront.SetFaceVisible(VoxelFace::RIGHT, true);
            }
        }
    }
}
}; // namespace Terrain
