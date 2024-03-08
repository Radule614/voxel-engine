#include "World.hpp"

#include <vector>

namespace Terrain
{
World::World(GLCore::Utils::PerspectiveCameraController &cameraController)
    : m_ChunkMap({}), m_ChunkGenerationQueue(), m_ShouldGenerationRun(std::make_shared<bool>(false)),
      m_Mutex(std::mutex()), m_CameraController(cameraController)
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
    *m_ShouldGenerationRun = true;
    m_GenerationThread = std::thread([this] { this->Generate(); });
}

void World::CheckChunkEdges(Chunk &chunk, Chunk::Neighbours &neighbours)
{
    auto &voxelGrid = chunk.m_VoxelGrid;
    for (size_t x = 0; x < CHUNK_WIDTH; x++)
    {
        for (size_t y = 0; y < CHUNK_HEIGHT; y++)
        {
            Voxel &front = voxelGrid[x][CHUNK_WIDTH - 1][y];
            Voxel &back = voxelGrid[x][0][y];
            Voxel &right = voxelGrid[CHUNK_WIDTH - 1][x][y];
            Voxel &left = voxelGrid[0][x][y];

            if (neighbours.front != nullptr)
                CheckVoxelEdge(front, neighbours.front->m_VoxelGrid[x][0][y], VoxelFace::FRONT);
            if (neighbours.back != nullptr)
                CheckVoxelEdge(back, neighbours.back->m_VoxelGrid[x][CHUNK_WIDTH - 1][y], VoxelFace::BACK);
            if (neighbours.right != nullptr)
                CheckVoxelEdge(right, neighbours.right->m_VoxelGrid[0][x][y], VoxelFace::RIGHT);
            if (neighbours.left != nullptr)
                CheckVoxelEdge(left, neighbours.left->m_VoxelGrid[CHUNK_WIDTH - 1][x][y], VoxelFace::LEFT);
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
    const siv::PerlinNoise::seed_type seed = 123456u;
    const siv::PerlinNoise perlin{seed};

    while (*m_ShouldGenerationRun)
    {
        std::tuple<bool, MapPosition> tuple = FindNextChunkLocation();
        if (!std::get<0>(tuple))
            continue;
        MapPosition &pos = std::get<1>(tuple);

        auto chunk = std::make_shared<Chunk>(pos.Vector, perlin);
        m_ChunkMap.insert({pos, chunk});
        chunk->Generate();
        Chunk::Neighbours neighbours = GetNeighbours(*chunk);
        CheckChunkEdges(*chunk, neighbours);
        chunk->GenerateMesh();

        m_Mutex.lock();
        m_ChunkGenerationQueue.push(chunk);
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
        LOG_INFO("CHUNKS: " + std::to_string(m_ChunkMap.size()));
        m_Mutex.unlock();
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

std::tuple<bool, MapPosition> World::FindNextChunkLocation()
{
    int32_t maxDistance = 12;
    glm::vec3 cameraPosition = m_CameraController.GetCamera().GetPosition();
    glm::vec2 offset =
        glm::vec2(glm::floor(cameraPosition.x / CHUNK_WIDTH), glm::floor(cameraPosition.z / CHUNK_WIDTH));
    for (int32_t r = 0; r < maxDistance; ++r)
    {
        for (int32_t x = 0; x <= r; ++x)
        {
            glm::vec2 locations[8] = {glm::vec2(x, -r),
                                      glm::vec2(x, r),
                                      glm::vec2(-r, x),
                                      glm::vec2(r, x),
                                      glm::vec2(-x, -r),
                                      glm::vec2(-x, r),
                                      glm::vec2(-r, -x),
                                      glm::vec2(r, -x)};
            for (size_t i = 0; i < 8; ++i)
            {
                if (glm::length(locations[i]) > maxDistance)
                    continue;
                glm::vec2 p = locations[i] + offset;
                MapPosition pos = MapPosition(glm::vec3(p.x, -1, p.y));
                auto chunk = m_ChunkMap.find(pos);
                if (chunk == m_ChunkMap.end())
                    return std::tuple<bool, MapPosition>(true, pos);
            }
        }
    }
    return std::tuple<bool, MapPosition>(false, MapPosition(glm::vec3(0)));
}
}; // namespace Terrain
