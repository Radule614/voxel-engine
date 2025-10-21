#pragma once

#include <map>
#include <queue>
#include <mutex>
#include <thread>
#include <glm/glm.hpp>
#include <GLCoreUtils.hpp>

#include "../../WorldSettings.hpp"
#include "Chunk.hpp"

namespace VoxelEngine
{

class World
{
public:
    World(const std::shared_ptr<GLCore::Utils::PerspectiveCameraController>& cameraController,
          WorldSettings&& settings);
    ~World();
    World(const World&) = delete;
    World& operator=(const World&) = delete;

    static Position2D GlobalToChunkSpace(const glm::i32vec3& pos);
    static std::pair<Position2D, Position3D> GlobalToWorldSpace(glm::i32vec3 pos);
    static glm::i32vec3 WorldToGlobalSpace(Position2D chunkPosition, Position3D positionInChunk);

    void StartGeneration();
    void StopGeneration();
    void Reset();

    std::vector<std::pair<Position2D, Chunk* > > FindDistantChunks();
    void RemoveChunk(Position2D position);

    const std::map<Position2D, std::unique_ptr<Chunk> >& GetChunkMap() const;
    std::map<Position2D, Chunk*>& GetRenderQueue();
    std::mutex& GetLock();
    std::map<Position2D, std::queue<Voxel> >& GetDeferredUpdateQueueMap();

private:
    static void SyncMeshWithNeighbour(Chunk& chunk, std::map<Position2D, Chunk* >& neighbours);
    static void SyncMeshWithNeighbour(Voxel& v1, Voxel& v2, VoxelFace face);

    static void SyncRadianceWithNeighbour(Chunk& chunk, std::map<Position2D, Chunk* >& neighbours);
    static void SyncRadianceWithNeighbour(Voxel& v1, Voxel& v2, Chunk& c1, Chunk& c2, VoxelFace face);

    std::vector<Position2D> GetNextChunkPositionBatch(Position2D center, int32_t batchSize) const;
    bool IsChunkPositionValidInBatch(const std::vector<Position2D>& batchPositions, Position2D newPosition) const;

    void GetNeighbours(const Chunk& chunk, std::map<Position2D, Chunk* >& neighbours);

    void GenerateWorld();
    void GenerateChunk(Position2D position);

private:
    std::map<Position2D, std::unique_ptr<Chunk> > m_ChunkMap;
    std::map<Position2D, Chunk*> m_RenderQueue;
    std::map<Position2D, std::queue<Voxel> > m_DeferredUpdateQueueMap;

    std::shared_ptr<GLCore::Utils::PerspectiveCameraController> m_CameraController;

    std::thread m_GenerationThread;
    bool m_ShouldGenerationRun;
    std::mutex m_Lock;

    WorldSettings m_Settings;
};

};
