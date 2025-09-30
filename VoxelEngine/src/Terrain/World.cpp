#include "World.hpp"

#include <GLCore.hpp>
#include <utility>
#include <vector>

namespace VoxelEngine
{

World::World(const std::shared_ptr<GLCore::Utils::PerspectiveCameraController>& cameraController,
             Settings&& settings)
    : m_ChunkMap({}),
      m_CameraController(cameraController),
      m_Biome(std::make_unique<Biome>(6512u, 6512u)),
      m_ShouldGenerationRun(false),
      m_Mutex(std::mutex()),
      m_Settings(std::move(settings))
{
}

World::~World() { StopGeneration(); }

void World::StartGeneration()
{
    m_ShouldGenerationRun = true;
    m_GenerationThread = std::thread([this] { this->GenerateWorld(); });
}

void World::SyncUpdatesWithNeighbours(Chunk& chunk, const Chunk::Neighbours& neighbours, const bool shouldSyncFaces)
{
    auto& voxelGrid = chunk.GetVoxelGrid();

    for (size_t x = 0; x < CHUNK_WIDTH; x++)
    {
        for (size_t y = 0; y < CHUNK_HEIGHT; y++)
        {
            if (neighbours.front != nullptr)
            {
                Voxel& v = voxelGrid[x][CHUNK_WIDTH - 1][y];
                Voxel& n = neighbours.front->GetVoxelGrid()[x][0][y];

                SyncRadianceWithNeighbour(v, n, chunk, *neighbours.front, FRONT);
                if (shouldSyncFaces)
                    SyncVisibleFacesWithNeighbour(v, n, FRONT);
            }
            if (neighbours.back != nullptr)
            {
                Voxel& v = voxelGrid[x][0][y];
                Voxel& n = neighbours.back->GetVoxelGrid()[x][CHUNK_WIDTH - 1][y];

                SyncRadianceWithNeighbour(v, n, chunk, *neighbours.back, BACK);
                if (shouldSyncFaces)
                    SyncVisibleFacesWithNeighbour(v, neighbours.back->GetVoxelGrid()[x][CHUNK_WIDTH - 1][y], BACK);
            }
            if (neighbours.right != nullptr)
            {
                Voxel& v = voxelGrid[CHUNK_WIDTH - 1][x][y];
                Voxel& n = neighbours.right->GetVoxelGrid()[0][x][y];

                SyncRadianceWithNeighbour(v, n, chunk, *neighbours.right, RIGHT);
                if (shouldSyncFaces)
                    SyncVisibleFacesWithNeighbour(v, n, RIGHT);
            }
            if (neighbours.left != nullptr)
            {
                Voxel& v = voxelGrid[0][x][y];
                Voxel& n = neighbours.left->GetVoxelGrid()[CHUNK_WIDTH - 1][x][y];

                SyncRadianceWithNeighbour(v, n, chunk, *neighbours.left, LEFT);
                if (shouldSyncFaces)
                    SyncVisibleFacesWithNeighbour(v, neighbours.left->GetVoxelGrid()[CHUNK_WIDTH - 1][x][y], LEFT);
            }
        }
    }

    chunk.CommitRadianceChanges();

    if (neighbours.front != nullptr)
    {
        neighbours.front->CommitRadianceChanges();
        if (shouldSyncFaces)
            neighbours.front->GenerateEdgeMesh(BACK);
    }
    if (neighbours.back != nullptr)
    {
        neighbours.back->CommitRadianceChanges();
        if (shouldSyncFaces)
            neighbours.back->GenerateEdgeMesh(FRONT);
    }
    if (neighbours.right != nullptr)
    {
        neighbours.right->CommitRadianceChanges();
        if (shouldSyncFaces)
            neighbours.right->GenerateEdgeMesh(LEFT);
    }
    if (neighbours.left != nullptr)
    {
        neighbours.left->CommitRadianceChanges();
        if (shouldSyncFaces)
            neighbours.left->GenerateEdgeMesh(RIGHT);
    }
}

void World::SyncVisibleFacesWithNeighbour(Voxel& v1, Voxel& v2, const VoxelFace face)
{
    if (!v1.IsTransparent() && v2.IsTransparent())
        v1.SetFaceVisible(face, true);
    else if (v1.IsTransparent() && !v2.IsTransparent())
        v2.SetFaceVisible(Voxel::GetOppositeFace(face), true);
}

void World::StopGeneration()
{
    m_ShouldGenerationRun = false;
    if (m_GenerationThread.joinable())
        m_GenerationThread.join();
}

Chunk::Neighbours World::GetNeighbours(const Chunk& chunk)
{
    const Position2D pos = chunk.GetPosition();
    const auto front = m_ChunkMap.find(Position2D(pos.x, pos.y + 1));
    const auto back = m_ChunkMap.find(Position2D(pos.x, pos.y - 1));
    const auto right = m_ChunkMap.find(Position2D(pos.x + 1, pos.y));
    const auto left = m_ChunkMap.find(Position2D(pos.x - 1, pos.y));

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

void World::SyncRadianceWithNeighbour(Voxel& v1, Voxel& v2, Chunk& c1, Chunk& c2, VoxelFace face)
{
    const glm::ivec3 p = static_cast<glm::ivec3>(v1.GetPosition()) + glm::ivec3(1);
    const glm::ivec3 np = static_cast<glm::ivec3>(v2.GetPosition()) + glm::ivec3(1);

    const int32_t radiance = c1.GetRadiance(p.x, p.z, p.y);
    const int32_t neighbourRadiance = c2.GetRadiance(np.x, np.z, np.y);

    if (face == FRONT)
    {
        c1.UpdateRadiance(p.x, p.z + 1, p.y, neighbourRadiance);
        c2.UpdateRadiance(np.x, np.z - 1, np.y, radiance);
    }

    if (face == BACK)
    {
        c1.UpdateRadiance(p.x, p.z - 1, p.y, neighbourRadiance);
        c2.UpdateRadiance(np.x, np.z + 1, np.y, radiance);
    }

    if (face == RIGHT)
    {
        c1.UpdateRadiance(p.x + 1, p.z, p.y, neighbourRadiance);
        c2.UpdateRadiance(np.x - 1, np.z, np.y, radiance);
    }

    if (face == LEFT)
    {
        c1.UpdateRadiance(p.x - 1, p.z, p.y, neighbourRadiance);
        c2.UpdateRadiance(np.x + 1, np.z, np.y, radiance);
    }
}

void World::GenerateWorld()
{
    while (m_ShouldGenerationRun)
    {
        Position2D center = GlobalToChunkSpace(m_CameraController->GetCamera().GetPosition());
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
    auto chunk = std::make_shared<Chunk>(*this, position, *m_Biome, m_Settings.StructureGenerators);
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

    const Chunk::Neighbours neighbours = GetNeighbours(*chunk);
    if (neighbours.front != nullptr)
        neighbours.front->GetLock().lock();
    if (neighbours.back != nullptr)
        neighbours.back->GetLock().lock();
    if (neighbours.right != nullptr)
        neighbours.right->GetLock().lock();
    if (neighbours.left != nullptr)
        neighbours.left->GetLock().lock();

    SyncUpdatesWithNeighbours(*chunk, neighbours);

    chunk->GenerateMesh();

    SyncUpdatesWithNeighbours(*chunk, neighbours, false);

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

std::queue<Position2D> World::FindNextChunkLocations(const Position2D center, const size_t count)
{
    const int32_t maxDistance = 10;
    std::queue<Position2D> positions = {};
    std::unordered_set<Position2D> existing = {};

    // TODO: refactor this
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

void World::Reset()
{
    m_ChunkMap.clear();
    m_ChangedChunks.clear();
    m_DeferredChunkQueueMap.clear();
}

bool World::IsPositionValid(const std::unordered_set<Position2D>& existing, const Position2D p)
{
    // TODO: refactor this
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

const std::map<Position2D, std::shared_ptr<Chunk> >& World::GetChunkMap() const { return m_ChunkMap; }

std::unordered_set<std::shared_ptr<Chunk> >& World::GetChangedChunks() { return m_ChangedChunks; }

std::mutex& World::GetLock() { return m_Mutex; }

std::map<Position2D, std::queue<Voxel> >& World::GetDeferredChunkQueue() { return m_DeferredChunkQueueMap; }

Position2D World::GlobalToChunkSpace(const glm::i32vec3& pos)
{
    return {
        static_cast<int16_t>(glm::floor((float_t) pos.x / CHUNK_WIDTH)),
        static_cast<int16_t>(glm::floor((float_t) pos.z / CHUNK_WIDTH))
    };
}

std::pair<Position2D, Position3D> World::GlobalToWorldSpace(const glm::i32vec3 pos)
{
    Position3D positionInChunk(pos.x % CHUNK_WIDTH, pos.y % CHUNK_HEIGHT, pos.z % CHUNK_WIDTH);
    Position2D chunkPosition = GlobalToChunkSpace(pos);

    return {chunkPosition, positionInChunk};
}

glm::i32vec3 World::WorldToGlobalSpace(const Position2D chunkPosition, const Position3D positionInChunk)
{
    const int32_t x = chunkPosition.x * CHUNK_WIDTH + positionInChunk.GetX();
    const int32_t y = positionInChunk.y;
    const int32_t z = chunkPosition.y * CHUNK_WIDTH + positionInChunk.GetZ();

    return {x, y, z};
}

}
