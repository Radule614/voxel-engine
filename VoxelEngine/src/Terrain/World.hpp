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
    World(GLCore::Utils::PerspectiveCameraController &cameraController);
    ~World();

    const std::map<Position2D, std::shared_ptr<Chunk>> &GetChunkMap() const;
    std::unordered_set<std::shared_ptr<Chunk>> &GetChangedChunks();

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

    void GenerateWorld();
    void GenerateChunk(Position2D position);
    std::queue<Position2D> FindNextChunkLocations(Position2D center, size_t count);
    bool IsPositionValid(std::unordered_set<Position2D> &existing, Position2D p);
    Position2D WorldToChunkSpace(const glm::vec3& pos);

private:
    std::map<Position2D, std::shared_ptr<Chunk>> m_ChunkMap;
    std::unordered_set<std::shared_ptr<Chunk>> m_ChangedChunks;
    GLCore::Utils::PerspectiveCameraController &m_CameraController;

    siv::PerlinNoise m_Perlin;

    std::thread m_GenerationThread;
    std::shared_ptr<bool> m_ShouldGenerationRun;
    std::mutex m_Mutex;
};
}; // namespace Terrain
