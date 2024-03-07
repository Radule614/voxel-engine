#include "World.hpp"

#include <vector>
#include <GLCoreUtils.hpp>
#include <thread>

namespace Terrain
{
World::World()
    : m_ChunkMap({}), m_ChunkGenerationQueue(), m_ShouldGenerationRun(std::make_shared<bool>(false)),
      m_Mutex(std::mutex())
{
}

World::~World()
{
}

const std::map<MapPosition, std::shared_ptr<Chunk>> &World::GetChunkMap() const
{
    return m_ChunkMap;
}

std::queue<std::shared_ptr<Chunk>> &World::GetChunkGenerationQueue()
{
    return m_ChunkGenerationQueue;
}

void World::StartGeneration()
{
    *m_ShouldGenerationRun = false;
    m_GenerationThread = std::thread([this] { this->Generate(); });
}

void World::CheckChunkEdges(Chunk &chunk, Chunk::Neighbours &neighbours)
{
    auto &voxelGrid = chunk.GetVoxelGrid();

    std::vector<std::vector<std::vector<Voxel>>> *zPositiveGrid = nullptr;
    std::vector<std::vector<std::vector<Voxel>>> *zNegativeGrid = nullptr;
    std::vector<std::vector<std::vector<Voxel>>> *xPositiveGrid = nullptr;
    std::vector<std::vector<std::vector<Voxel>>> *xNegativeGrid = nullptr;

    if (neighbours.front != nullptr)
        zPositiveGrid = &neighbours.front->GetVoxelGrid();
    if (neighbours.back != nullptr)
        zNegativeGrid = &neighbours.back->GetVoxelGrid();
    if (neighbours.right != nullptr)
        xPositiveGrid = &neighbours.right->GetVoxelGrid();
    if (neighbours.left != nullptr)
        xNegativeGrid = &neighbours.left->GetVoxelGrid();

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

void World::Generate()
{
    // LOG_INFO("Generation thread started");
    const siv::PerlinNoise::seed_type seed = 123456u;
    const siv::PerlinNoise perlin{seed};
    int32_t size = 4;

    for (int32_t i = -size; i <= size; ++i)
    {
        for (int32_t j = -size; j <= size; ++j)
        {
            if (*m_ShouldGenerationRun)
                return;
            MapPosition pos(glm::vec3(i, -1, j));
            auto chunk = std::make_shared<Chunk>(pos.Vector, perlin);
            m_ChunkMap.insert({pos, chunk});
            chunk->Generate();
            Chunk::Neighbours neighbours = GetNeighbours(*chunk);
            CheckChunkEdges(*chunk, neighbours);
            chunk->GenerateMesh();

            m_Mutex.lock();
            if (neighbours.front != nullptr)
            {
                neighbours.front->GenerateMesh();
                m_ChunkGenerationQueue.push(neighbours.front);
            }
            if (neighbours.back != nullptr)
            {
                neighbours.back->GenerateMesh();
                m_ChunkGenerationQueue.push(neighbours.back);
            }
            if (neighbours.right != nullptr)
            {
                neighbours.right->GenerateMesh();
                m_ChunkGenerationQueue.push(neighbours.right);
            }
            if (neighbours.left != nullptr)
            {
                neighbours.left->GenerateMesh();
                m_ChunkGenerationQueue.push(neighbours.left);
            }
            m_ChunkGenerationQueue.push(chunk);
            m_Mutex.unlock();

            // LOG_INFO("Generation thread running...");
            // std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
}
void World::StopGeneration()
{
    *m_ShouldGenerationRun = false;
    if (m_GenerationThread.joinable())
        m_GenerationThread.join();
}

Chunk::Neighbours World::GetNeighbours(Chunk &chunk)
{
    glm::vec3 pos = chunk.GetPosition();
    auto front = m_ChunkMap.find(MapPosition(glm::vec3(pos.x, pos.y, pos.z + 1)));
    auto back = m_ChunkMap.find(MapPosition(glm::vec3(pos.x, pos.y, pos.z - 1)));
    auto right = m_ChunkMap.find(MapPosition(glm::vec3(pos.x + 1, pos.y, pos.z)));
    auto left = m_ChunkMap.find(MapPosition(glm::vec3(pos.x - 1, pos.y, pos.z)));

    Chunk::Neighbours neighbours = {};

    if (front != m_ChunkMap.end())
        neighbours.front = front->second;
    if (back != m_ChunkMap.end())
        neighbours.back = back->second;
    if (right != m_ChunkMap.end())
        neighbours.right = right->second;
    if (left != m_ChunkMap.end())
        neighbours.left = left->second;

    return neighbours;
}

MapPosition World::FindNextChunkLocation()
{
    int32_t size = 0;
    int32_t maxDistance = 5;
    for (size_t r = 0; r < maxDistance; ++r)
    {
        for(size_t x = -r; x <= r; ++x ){
            for(size_t z = -r; z <= r; ++z ){
                MapPosition pos = MapPosition(glm::vec3(x, -1, z));
                auto chunk = m_ChunkMap.find(pos);
                if(chunk == m_ChunkMap.end())
                    return pos;
            }
        }
    }
    return glm::vec3(0, 0, 0);
}
}; // namespace Terrain
