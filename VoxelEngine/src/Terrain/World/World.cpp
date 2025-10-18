#include "World.hpp"

#include <GLCore.hpp>
#include <utility>
#include <vector>

namespace VoxelEngine
{

World::World(const std::shared_ptr<GLCore::Utils::PerspectiveCameraController>& cameraController,
             WorldSettings&& settings)
    : m_ChunkMap({}),
      m_CameraController(cameraController),
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

void World::StopGeneration()
{
    m_ShouldGenerationRun = false;
    if (m_GenerationThread.joinable())
        m_GenerationThread.join();
}

void World::Reset()
{
    m_ChunkMap.clear();
    m_RenderQueue.clear();
    m_DeferredUpdateQueueMap.clear();
}

void World::GenerateWorld()
{
    while (m_ShouldGenerationRun)
    {
        const Position2D center = GlobalToChunkSpace(m_CameraController->GetCamera().GetPosition());
        std::vector<Position2D> batch = GetNextChunkPositionBatch(center, TerrainConfig::ThreadCount);
        std::vector<std::thread> threads = {};

        for (auto position: batch)
            threads.emplace_back([this, position] { this->GenerateChunk(position); });

        for (auto& thread: threads)
        {
            if (thread.joinable())
                thread.join();
        }
    }
}

void World::GenerateChunk(Position2D position)
{
    auto chunk = std::make_shared<Chunk>(*this, position, *m_Settings.m_Biome);
    chunk->GetLock().lock();

    m_ChunkMap.insert({position, chunk});
    chunk->Generate();

    auto deferredQueueMap = m_DeferredUpdateQueueMap.find(position);
    if (deferredQueueMap != m_DeferredUpdateQueueMap.end())
    {
        m_Mutex.lock();
        auto& deferredQueue = deferredQueueMap->second;

        while (!deferredQueue.empty())
        {
            Voxel& voxelFromQueue = deferredQueue.front();
            Voxel& voxel = chunk->GetVoxelFromGrid(voxelFromQueue.GetPosition());

            voxel.SetPosition(voxelFromQueue.GetPosition());
            voxel.SetVoxelType(voxelFromQueue.GetVoxelType());

            deferredQueue.pop();
        }
        m_Mutex.unlock();
    }

    std::map<Position2D, std::shared_ptr<Chunk> > neighboursLevel1{};
    GetNeighbours(*chunk, neighboursLevel1);

    for (const auto& [_, neighbour]: neighboursLevel1)
        neighbour->GetLock().lock();

    SyncMeshWithNeighbour(*chunk, neighboursLevel1);

    chunk->GenerateMesh();

    m_Mutex.lock();

    if (neighboursLevel1.size() == 8)
    {
        chunk->InitRadiance();
        SyncRadianceWithNeighbour(*chunk, neighboursLevel1);
        m_RenderQueue.insert(chunk);
    }

    for (const auto& [_, level1]: neighboursLevel1)
    {
        std::map<Position2D, std::shared_ptr<Chunk> > neighboursLevel2{};
        GetNeighbours(*level1, neighboursLevel2);

        if (neighboursLevel2.size() == 8)
        {
            level1->InitRadiance();

            SyncRadianceWithNeighbour(*level1, neighboursLevel2);

            for (const auto& [_, level2]: neighboursLevel2)
            {
                std::map<Position2D, std::shared_ptr<Chunk> > neighboursLayer3{};
                GetNeighbours(*level2, neighboursLayer3);

                if (neighboursLayer3.size() == 8)
                    m_RenderQueue.insert(level2);
            }

            m_RenderQueue.insert(level1);
        }

        level1->GetLock().unlock();
    }

    m_Mutex.unlock();
    chunk->GetLock().unlock();
}

void World::SyncMeshWithNeighbour(Chunk& chunk, std::map<Position2D, std::shared_ptr<Chunk> >& neighbours)
{
    auto& voxelGrid = chunk.GetVoxelGrid();

    const auto front = neighbours.find(Position2D(0, 1));
    const auto back = neighbours.find(Position2D(0, -1));
    const auto right = neighbours.find(Position2D(1, 0));
    const auto left = neighbours.find(Position2D(-1, 0));

    for (size_t x = 0; x < CHUNK_WIDTH; x++)
    {
        for (size_t y = 0; y < CHUNK_HEIGHT; y++)
        {
            if (front != neighbours.end())
            {
                Chunk& neighbour = *front->second;
                Voxel& v = voxelGrid[x][CHUNK_WIDTH - 1][y];
                Voxel& n = neighbour.GetVoxelGrid()[x][0][y];

                SyncMeshWithNeighbour(v, n, FRONT);
            }

            if (back != neighbours.end())
            {
                Chunk& neighbour = *back->second;
                Voxel& v = voxelGrid[x][0][y];
                Voxel& n = neighbour.GetVoxelGrid()[x][CHUNK_WIDTH - 1][y];

                SyncMeshWithNeighbour(v, n, BACK);
            }

            if (right != neighbours.end())
            {
                Chunk& neighbour = *right->second;
                Voxel& v = voxelGrid[CHUNK_WIDTH - 1][x][y];
                Voxel& n = neighbour.GetVoxelGrid()[0][x][y];

                SyncMeshWithNeighbour(v, n, RIGHT);
            }

            if (left != neighbours.end())
            {
                Chunk& neighbour = *left->second;
                Voxel& v = voxelGrid[0][x][y];
                Voxel& n = neighbour.GetVoxelGrid()[CHUNK_WIDTH - 1][x][y];

                SyncMeshWithNeighbour(v, n, LEFT);
            }
        }
    }

    if (front != neighbours.end())
        front->second->GenerateEdgeMesh(BACK);
    if (back != neighbours.end())
        back->second->GenerateEdgeMesh(FRONT);
    if (right != neighbours.end())
        right->second->GenerateEdgeMesh(LEFT);
    if (left != neighbours.end())
        left->second->GenerateEdgeMesh(RIGHT);
}

void World::SyncRadianceWithNeighbour(Chunk& chunk, std::map<Position2D, std::shared_ptr<Chunk> >& neighbours)
{
    auto& voxelGrid = chunk.GetVoxelGrid();

    const auto front = neighbours.find(Position2D(0, 1));
    const auto back = neighbours.find(Position2D(0, -1));
    const auto right = neighbours.find(Position2D(1, 0));
    const auto left = neighbours.find(Position2D(-1, 0));

    for (size_t x = 0; x < CHUNK_WIDTH; x++)
    {
        for (size_t y = 0; y < CHUNK_HEIGHT; y++)
        {
            if (front != neighbours.end())
            {
                Chunk& neighbour = *front->second;
                Voxel& v = voxelGrid[x][CHUNK_WIDTH - 1][y];
                Voxel& n = neighbour.GetVoxelGrid()[x][0][y];

                SyncRadianceWithNeighbour(v, n, chunk, neighbour, FRONT);
            }

            if (back != neighbours.end())
            {
                Chunk& neighbour = *back->second;
                Voxel& v = voxelGrid[x][0][y];
                Voxel& n = neighbour.GetVoxelGrid()[x][CHUNK_WIDTH - 1][y];

                SyncRadianceWithNeighbour(v, n, chunk, neighbour, BACK);
            }

            if (right != neighbours.end())
            {
                Chunk& neighbour = *right->second;
                Voxel& v = voxelGrid[CHUNK_WIDTH - 1][x][y];
                Voxel& n = neighbour.GetVoxelGrid()[0][x][y];

                SyncRadianceWithNeighbour(v, n, chunk, neighbour, RIGHT);
            }

            if (left != neighbours.end())
            {
                Chunk& neighbour = *left->second;
                Voxel& v = voxelGrid[0][x][y];
                Voxel& n = neighbour.GetVoxelGrid()[CHUNK_WIDTH - 1][x][y];

                SyncRadianceWithNeighbour(v, n, chunk, neighbour, LEFT);
            }
        }
    }

    chunk.CommitRadianceChanges();

    for (const auto& [_, neighbour]: neighbours)
        neighbour->CommitRadianceChanges();
}

void World::SyncMeshWithNeighbour(Voxel& v1, Voxel& v2, const VoxelFace face)
{
    if (!v1.IsAir() && v2.IsAir())
        v1.SetFaceVisible(face, true);
    else if (v1.IsAir() && !v2.IsAir())
        v2.SetFaceVisible(Voxel::GetOppositeFace(face), true);
}

void World::GetNeighbours(const Chunk& chunk, std::map<Position2D, std::shared_ptr<Chunk> >& neighbours)
{
    const Position2D position = chunk.GetPosition();
    const int dirs[8][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};

    for (const auto& [dx, dy]: dirs)
    {
        const auto neighbour = m_ChunkMap.find(Position2D(position.x + dx, position.y + dy));

        if (neighbour != m_ChunkMap.end())
            neighbours.emplace(Position2D(dx, dy), neighbour->second);
    }
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

std::vector<Position2D> World::GetNextChunkPositionBatch(const Position2D center, const int32_t batchSize) const
{
    int32_t maxDistance = TerrainConfig::MaxChunkDistance;

    std::vector<Position2D> batch = {};
    batch.reserve(batchSize);

    for (int32_t r = 0; r < maxDistance; ++r)
    {
        for (int32_t x = 0; x <= r; ++x)
        {
            const int offsets[8][2] = {{x, -r}, {x, r}, {-r, x}, {r, x}, {-x, -r}, {-x, r}, {-r, -x}, {r, -x}};

            for (auto& [dx, dy]: offsets)
            {
                Position2D position = center + Position2D(dx, dy);
                if (GetDistance(position, center) > maxDistance || !IsChunkPositionValidInBatch(batch, position))
                    continue;

                batch.emplace_back(position);

                if (batch.size() == batchSize)
                    return batch;
            }
        }
    }
    return batch;
}

bool World::IsChunkPositionValidInBatch(const std::vector<Position2D>& batchPositions,
                                        const Position2D newPosition) const
{
    for (const auto existingPosition: batchPositions)
    {
        if (GetDistance(existingPosition, newPosition) < 4.0f)
            return false;
    }

    return !m_ChunkMap.contains(newPosition);
}

const std::map<Position2D, std::shared_ptr<Chunk> >& World::GetChunkMap() const { return m_ChunkMap; }

std::unordered_set<std::shared_ptr<Chunk> >& World::GetRenderQueue() { return m_RenderQueue; }

std::mutex& World::GetLock() { return m_Mutex; }

std::map<Position2D, std::queue<Voxel> >& World::GetDeferredUpdateQueueMap() { return m_DeferredUpdateQueueMap; }

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
    const int32_t y = positionInChunk.GetY();
    const int32_t z = chunkPosition.y * CHUNK_WIDTH + positionInChunk.GetZ();

    return {x, y, z};
}

}
