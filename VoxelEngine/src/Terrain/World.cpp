#include "World.hpp"

#include <GLCore.hpp>
#include <vector>

namespace VoxelEngine
{

World::World(const std::shared_ptr<GLCore::Utils::PerspectiveCameraController>& cameraController)
    : m_ChunkMap({}),
      m_CameraController(cameraController),
      m_Perlin(6512u),
      m_ShouldGenerationRun(std::make_shared<bool>(false)),
      m_Mutex(std::mutex())
{
}

World::~World()
{
    StopGeneration();
}

void World::StartGeneration()
{
    *m_ShouldGenerationRun = true;
    m_GenerationThread = std::thread([this] { this->GenerateWorld(); });
}

void World::CheckChunkEdges(Chunk& chunk, Chunk::Neighbours& neighbours)
{
    auto& voxelGrid = chunk.GetVoxelGrid();
    for (size_t x = 0; x < CHUNK_WIDTH; x++)
    {
        for (size_t y = 0; y < CHUNK_HEIGHT; y++)
        {
            Voxel& front = voxelGrid[x][CHUNK_WIDTH - 1][y];
            Voxel& back = voxelGrid[x][0][y];
            Voxel& right = voxelGrid[CHUNK_WIDTH - 1][x][y];
            Voxel& left = voxelGrid[0][x][y];

            if (neighbours.front != nullptr)
                CheckVoxelEdge(front, neighbours.front->GetVoxelGrid()[x][0][y], FRONT);
            if (neighbours.back != nullptr)
                CheckVoxelEdge(back, neighbours.back->GetVoxelGrid()[x][CHUNK_WIDTH - 1][y], BACK);
            if (neighbours.right != nullptr)
                CheckVoxelEdge(right, neighbours.right->GetVoxelGrid()[0][x][y], RIGHT);
            if (neighbours.left != nullptr)
                CheckVoxelEdge(left, neighbours.left->GetVoxelGrid()[CHUNK_WIDTH - 1][x][y], LEFT);
        }
    }
}

void World::CheckVoxelEdge(Voxel& v1, Voxel& v2, VoxelFace face)
{
    if (!v1.IsTransparent() && v2.IsTransparent())
        v1.SetFaceVisible(face, true);
    else if (v1.IsTransparent() && !v2.IsTransparent())
        v2.SetFaceVisible(Voxel::GetOppositeFace(face), true);
}

void World::StopGeneration()
{
    *m_ShouldGenerationRun = false;
    if (m_GenerationThread.joinable())
        m_GenerationThread.join();
}

Chunk::Neighbours World::GetNeighbours(Chunk& chunk)
{
    Position2D pos = chunk.GetPosition();
    auto front = m_ChunkMap.find(Position2D(pos.x, pos.y + 1));
    auto back = m_ChunkMap.find(Position2D(pos.x, pos.y - 1));
    auto right = m_ChunkMap.find(Position2D(pos.x + 1, pos.y));
    auto left = m_ChunkMap.find(Position2D(pos.x - 1, pos.y));

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
        Position2D center = WorldToChunkSpace(m_CameraController->GetCamera().GetPosition());
        std::queue<Position2D> chunkLocations = FindNextChunkLocations(center, TerrainConfig::ThreadCount);
        std::vector<std::thread> threads = {};

        while (!chunkLocations.empty())
        {
            Position2D pos = chunkLocations.front();
            threads.emplace_back([this, pos] { this->GenerateChunk(pos); });
            chunkLocations.pop();
        }

        for (auto& thread: threads)
        {
            if (thread.joinable())
                thread.join();
        }
    }
}

void World::GenerateChunk(Position2D position)
{
    auto chunk = std::make_shared<Chunk>(*this, position, m_Perlin);
    chunk->GetLock().lock();
    m_ChunkMap.insert({position, chunk});
    chunk->Generate();

    auto deferredQueueMap = m_DeferredChunkQueueMap.find(position);
    if (deferredQueueMap != m_DeferredChunkQueueMap.end())
    {
        m_Mutex.lock();
        auto& deferredQueue = deferredQueueMap->second;
        while (!deferredQueue.empty())
        {
            Voxel& v = deferredQueue.front();
            auto& voxelGrid = chunk->GetVoxelGrid();
            const Position3D& p = v.GetPosition();
            voxelGrid[p.GetX()][p.GetZ()][p.y].SetPosition(p);
            voxelGrid[p.GetX()][p.GetZ()][p.y].SetVoxelType(v.GetVoxelType());
            deferredQueue.pop();
        }
        m_Mutex.unlock();
    }

    Chunk::Neighbours neighbours = GetNeighbours(*chunk);
    if (neighbours.front != nullptr)
        neighbours.front->GetLock().lock();
    if (neighbours.back != nullptr)
        neighbours.back->GetLock().lock();
    if (neighbours.right != nullptr)
        neighbours.right->GetLock().lock();
    if (neighbours.left != nullptr)
        neighbours.left->GetLock().lock();

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
    m_ChangedChunks.insert(chunk);
    if (neighbours.front != nullptr)
    {
        m_ChangedChunks.insert(neighbours.front);
        neighbours.front->GetLock().unlock();
    }
    if (neighbours.back != nullptr)
    {
        m_ChangedChunks.insert(neighbours.back);
        neighbours.back->GetLock().unlock();
    }
    if (neighbours.right != nullptr)
    {
        m_ChangedChunks.insert(neighbours.right);
        neighbours.right->GetLock().unlock();
    }
    if (neighbours.left != nullptr)
    {
        m_ChangedChunks.insert(neighbours.left);
        neighbours.left->GetLock().unlock();
    }
    m_Mutex.unlock();
    chunk->GetLock().unlock();
}

std::queue<Position2D> World::FindNextChunkLocations(Position2D center, size_t count)
{
    int32_t maxDistance = 20;
    std::queue<Position2D> positions = {};
    std::unordered_set<Position2D> existing = {};
    for (int32_t r = 0; r < maxDistance; ++r)
    {
        for (int32_t x = 0; x <= r; ++x)
        {
            Position2D locations[8] = {
                Position2D(x, -r),
                Position2D(x, r),
                Position2D(-r, x),
                Position2D(r, x),
                Position2D(-x, -r),
                Position2D(-x, r),
                Position2D(-r, -x),
                Position2D(r, -x)
            };
            for (auto& location: locations)
            {
                if (location.GetLength() > maxDistance)
                    continue;
                Position2D p = location + center;
                if (!IsPositionValid(existing, p))
                    continue;
                auto pos = Position2D(p.x, p.y);
                if (auto chunk = m_ChunkMap.find(pos); chunk == m_ChunkMap.end() && !existing.contains(pos))
                {
                    existing.insert(p);
                    positions.push(pos);
                }
                if (existing.size() == count)
                    return positions;
            }
        }
    }
    return positions;
}

bool World::IsPositionValid(std::unordered_set<Position2D>& existing, Position2D p)
{
    Position2D locations[12] = {
        Position2D(p.x, p.y + 2),
        Position2D(p.x, p.y + 1),
        Position2D(p.x, p.y - 1),
        Position2D(p.x, p.y - 2),
        Position2D(p.x + 2, p.y),
        Position2D(p.x + 1, p.y),
        Position2D(p.x - 1, p.y),
        Position2D(p.x - 2, p.y),
        Position2D(p.x + 1, p.y + 1),
        Position2D(p.x + 1, p.y - 1),
        Position2D(p.x - 1, p.y + 1),
        Position2D(p.x - 1, p.y - 1)
    };
    for (auto location: locations)
        if (existing.contains(location))
            return false;
    return true;
}

Position2D World::WorldToChunkSpace(const glm::vec3& pos)
{
    return {
        static_cast<int16_t>(glm::floor(pos.x / CHUNK_WIDTH)),
        static_cast<int16_t>(glm::floor(pos.z / CHUNK_WIDTH))
    };
}

const std::map<Position2D, std::shared_ptr<Chunk> >& World::GetChunkMap() const
{
    return m_ChunkMap;
}

std::unordered_set<std::shared_ptr<Chunk> >& World::GetChangedChunks()
{
    return m_ChangedChunks;
}

std::mutex& World::GetLock()
{
    return m_Mutex;
}

std::map<Position2D, std::queue<Voxel> >& World::GetDeferredChunkQueue()
{
    return m_DeferredChunkQueueMap;
}

std::pair<Position2D, Position3D> World::GetPositionInWorld(glm::i16vec3 pos) const
{
    if (InRange(pos.x, 0, CHUNK_WIDTH - 1) && InRange(pos.y, 0, CHUNK_WIDTH - 1) && InRange(pos.z, 0, CHUNK_WIDTH - 1))
        return {Position2D(0, 0), Position3D(pos.x, pos.y, pos.z)};
    Position2D chunkPos(0, 0);
    while (pos.x < 0)
    {
        pos.x += CHUNK_WIDTH;
        --chunkPos.x;
    }
    while (pos.x > CHUNK_WIDTH - 1)
    {
        pos.x -= CHUNK_WIDTH;
        ++chunkPos.x;
    }
    while (pos.z < 0)
    {
        pos.z += CHUNK_WIDTH;
        --chunkPos.y;
    }
    while (pos.z > CHUNK_WIDTH - 1)
    {
        pos.z -= CHUNK_WIDTH;
        ++chunkPos.y;
    }
    return {chunkPos, Position3D(pos.x, pos.y, pos.z)};
}

};
