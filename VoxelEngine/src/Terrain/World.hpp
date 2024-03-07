#pragma once

#include <map>
#include <queue>
#include <glm/glm.hpp>

#include "Chunk.hpp"
#include "MapPosition.hpp"

#include <mutex>
#include <thread>

namespace Terrain
{
class World
{
public:
    World();
    ~World();

    const std::map<MapPosition, std::shared_ptr<Chunk>> &GetChunkMap() const;
    std::queue<std::shared_ptr<Chunk>> &GetChunkGenerationQueue();

    void StartGeneration();
    void StopGeneration();

    std::mutex &GetLock()
    {
        return m_Mutex;
    }

private:
    void CheckChunkEdges(Chunk &chunk, Chunk::Neighbours &neighbours);
    void CheckVoxelEdge(Voxel &v1, Voxel &v2, VoxelFace face);
    Chunk::Neighbours GetNeighbours(Chunk &chunk);
    void Generate();
    MapPosition FindNextChunkLocation();


private:
    std::map<MapPosition, std::shared_ptr<Chunk>> m_ChunkMap;
    std::queue<std::shared_ptr<Chunk>> m_ChunkGenerationQueue;

    std::thread m_GenerationThread;
    std::shared_ptr<bool> m_ShouldGenerationRun;
    std::mutex m_Mutex;
};
}; // namespace Terrain
