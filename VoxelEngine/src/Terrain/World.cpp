#include "World.hpp"

#include <GLCore.hpp>
#include <vector>

namespace Terrain
{
World::World(GLCore::Utils::PerspectiveCameraController &cameraController)
    : m_ChunkMap({}), m_ChunkGenerationQueue(), m_ShouldGenerationRun(std::make_shared<bool>(false)),
      m_Mutex(std::mutex()), m_CameraController(cameraController), m_Perlin(6512u)
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
    m_GenerationThread = std::thread([this] { this->GenerateWorld(); });
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

void World::StopGeneration()
{
    *m_ShouldGenerationRun = false;
    if (m_GenerationThread.joinable())
        m_GenerationThread.join();
}

Chunk::Neighbours World::GetNeighbours(Chunk &chunk)
{
    glm::vec2 pos = chunk.GetPosition();
    auto front = m_ChunkMap.find(MapPosition(glm::vec2(pos.x, pos.y + 1)));
    auto back = m_ChunkMap.find(MapPosition(glm::vec2(pos.x, pos.y - 1)));
    auto right = m_ChunkMap.find(MapPosition(glm::vec2(pos.x + 1, pos.y)));
    auto left = m_ChunkMap.find(MapPosition(glm::vec2(pos.x - 1, pos.y)));

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

void World::GenerateWorld()
{
    while (*m_ShouldGenerationRun)
    {
        std::vector<MapPosition> locations = FindNextChunkLocations();
        std::vector<std::thread> threads = {};
        for (size_t i = 0; i < locations.size(); ++i)
        {
            MapPosition pos = locations[i];
            threads.push_back(std::thread([this, pos] { this->GenerateChunk(pos); }));
        }
        for (size_t i = 0; i < locations.size(); ++i)
        {
            if (threads[i].joinable())
                threads[i].join();
        }
        LOG_INFO("Chunk size -> " + std::to_string(m_ChunkMap.size()));
    }
}

void World::GenerateChunk(MapPosition position)
{
    auto chunk = std::make_shared<Chunk>(position.Vector, m_Perlin);
    m_ChunkMap.insert({position, chunk});
    chunk->Generate();

    Chunk::Neighbours neighbours = GetNeighbours(*chunk);
    CheckChunkEdges(*chunk, neighbours);
    chunk->GenerateMesh();
    if (neighbours.front != nullptr)
        neighbours.front->GenerateEdgeMesh(VoxelFace::BACK);
    if (neighbours.back != nullptr)
        neighbours.back->GenerateEdgeMesh(VoxelFace::FRONT);
    if (neighbours.right != nullptr)
        neighbours.right->GenerateEdgeMesh(VoxelFace::LEFT);
    if (neighbours.left != nullptr)
        neighbours.left->GenerateEdgeMesh(VoxelFace::RIGHT);


    m_Mutex.lock();
    m_ChunkGenerationQueue.push(chunk);
    if (neighbours.front != nullptr)
        m_ChunkGenerationQueue.push(neighbours.front);
    if (neighbours.back != nullptr)
        m_ChunkGenerationQueue.push(neighbours.back);
    if (neighbours.right != nullptr)
        m_ChunkGenerationQueue.push(neighbours.right);
    if (neighbours.left != nullptr)
        m_ChunkGenerationQueue.push(neighbours.left);
    m_Mutex.unlock();
}

std::vector<MapPosition> World::FindNextChunkLocations()
{
    int32_t maxDistance = 20;
    glm::vec3 cameraPosition = m_CameraController.GetCamera().GetPosition();
    glm::vec2 offset =
        glm::vec2(glm::floor(cameraPosition.x / CHUNK_WIDTH), glm::floor(cameraPosition.z / CHUNK_WIDTH));
    std::vector<MapPosition> positions = {};
    std::unordered_set<glm::vec2> existing = {};
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
                if (!IsPositionValid(existing, p))
                    continue;
                MapPosition pos = MapPosition(p);
                auto chunk = m_ChunkMap.find(pos);
                if (chunk == m_ChunkMap.end() && std::find(positions.begin(), positions.end(), pos) == positions.end())
                {
                    existing.insert(p);
                    positions.push_back(pos);
                }
                if (positions.size() == THREADS)
                    return positions;
            }
        }
    }
    return positions;
}

bool World::IsPositionValid(std::unordered_set<glm::vec2> &existing, glm::vec2 p)
{
    glm::vec2 locations[12] = {glm::vec2(p.x, p.y + 2),
                               glm::vec2(p.x, p.y + 1),
                               glm::vec2(p.x, p.y - 1),
                               glm::vec2(p.x, p.y - 2),
                               glm::vec2(p.x + 2, p.y),
                               glm::vec2(p.x + 1, p.y),
                               glm::vec2(p.x - 1, p.y),
                               glm::vec2(p.x - 2, p.y),
                               glm::vec2(p.x + 1, p.y + 1),
                               glm::vec2(p.x + 1, p.y - 1),
                               glm::vec2(p.x - 1, p.y + 1),
                               glm::vec2(p.x - 1, p.y - 1)};
    for (size_t i = 0; i < 12; ++i)
        if (existing.find(locations[i]) != existing.end())
            return false;
    return true;
}
}; // namespace Terrain
