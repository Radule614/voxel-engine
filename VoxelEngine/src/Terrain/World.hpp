#pragma once

#include <map>
#include <unordered_set>
#include <queue>
#include <mutex>
#include <thread>
#include <glm/glm.hpp>
#include <GLCoreUtils.hpp>

#include "Chunk.hpp"
#include "Position2D.hpp"


namespace VoxelEngine
{

class World
{
public:
    World(const std::shared_ptr<GLCore::Utils::PerspectiveCameraController>& cameraController);
    ~World();

    void StartGeneration();
    void StopGeneration();

    const std::map<Position2D, std::shared_ptr<Chunk> >& GetChunkMap() const;
    std::unordered_set<std::shared_ptr<Chunk> >& GetChangedChunks();
    std::mutex& GetLock();
    std::map<Position2D, std::queue<Voxel> >& GetDeferredChunkQueue();
    std::pair<Position2D, Position3D> GetPositionInWorld(glm::i16vec3 pos) const;
    Position2D WorldToChunkSpace(const glm::vec3& pos);

private:
    void CheckChunkEdges(Chunk& chunk, Chunk::Neighbours& neighbours);
    void CheckVoxelEdge(Voxel& v1, Voxel& v2, VoxelFace face);
    Chunk::Neighbours GetNeighbours(Chunk& chunk);

    void GenerateWorld();
    void GenerateChunk(Position2D position);
    std::queue<Position2D> FindNextChunkLocations(Position2D center, size_t count);
    bool IsPositionValid(std::unordered_set<Position2D>& existing, Position2D p);

private:
    std::map<Position2D, std::shared_ptr<Chunk> > m_ChunkMap;
    std::unordered_set<std::shared_ptr<Chunk> > m_ChangedChunks;
    std::map<Position2D, std::queue<Voxel> > m_DeferredChunkQueueMap;
    std::shared_ptr<GLCore::Utils::PerspectiveCameraController> m_CameraController;

    siv::PerlinNoise m_Perlin;

    std::thread m_GenerationThread;
    std::shared_ptr<bool> m_ShouldGenerationRun;
    std::mutex m_Mutex;
};

};
