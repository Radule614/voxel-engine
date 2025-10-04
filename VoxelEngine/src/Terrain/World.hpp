#pragma once

#include <map>
#include <unordered_set>
#include <queue>
#include <mutex>
#include <thread>
#include <glm/glm.hpp>
#include <GLCoreUtils.hpp>

#include "Chunk.hpp"

namespace VoxelEngine
{

class World
{
public:
    struct Settings
    {
    };

    World(const std::shared_ptr<GLCore::Utils::PerspectiveCameraController>& cameraController, Settings settings);
    ~World();

    static Position2D GlobalToChunkSpace(const glm::i32vec3& pos);
    static std::pair<Position2D, Position3D> GlobalToWorldSpace(glm::i32vec3 pos);
    static glm::i32vec3 WorldToGlobalSpace(Position2D chunkPosition, Position3D positionInChunk);

    void StartGeneration();
    void StopGeneration();
    void Reset();

    const std::map<Position2D, std::shared_ptr<Chunk> >& GetChunkMap() const;
    std::unordered_set<std::shared_ptr<Chunk> >& GetChangedChunks();
    std::mutex& GetLock();
    std::map<Position2D, std::queue<Voxel> >& GetDeferredChunkQueue();

private:
    static void SyncVisibleFacesWithNeighbour(Voxel& v1, Voxel& v2, VoxelFace face);
    static void SyncRadianceWithNeighbour(Voxel& v1, Voxel& v2, Chunk& c1, Chunk& c2, VoxelFace face);
    static void SyncUpdatesWithNeighbours(Chunk& chunk,
                                          const Chunk::Neighbours& neighbours,
                                          bool shouldSyncFaces = true);
    static bool IsPositionValid(const std::unordered_set<Position2D>& existing, Position2D p);

    Chunk::Neighbours GetNeighbours(const Chunk& chunk);

    void GenerateWorld();
    void GenerateChunk(Position2D position);
    std::queue<Position2D> FindNextChunkLocations(Position2D center, size_t count);

private:
    std::map<Position2D, std::shared_ptr<Chunk> > m_ChunkMap;
    std::unordered_set<std::shared_ptr<Chunk> > m_ChangedChunks;
    std::map<Position2D, std::queue<Voxel> > m_DeferredChunkQueueMap;

    std::shared_ptr<GLCore::Utils::PerspectiveCameraController> m_CameraController;

    std::unique_ptr<Biome> m_Biome;

    std::thread m_GenerationThread;
    bool m_ShouldGenerationRun;
    std::mutex m_Mutex;

    Settings m_Settings;
};

};
