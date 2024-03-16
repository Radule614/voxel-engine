#pragma once

#include <map>
#include <unordered_set>
#include <queue>
#include <mutex>
#include <thread>
#include <glm/glm.hpp>
#include <GLCoreUtils.hpp>

#include "Chunk.hpp"
#include "MapPosition.hpp"


namespace Terrain
{
class World
{
public:
    World(GLCore::Utils::PerspectiveCameraController &cameraController);
    ~World();

    const std::map<MapPosition, std::shared_ptr<Chunk>> &GetChunkMap() const;
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
    void GenerateChunk(MapPosition position);
    std::queue<MapPosition> FindNextChunkLocations(glm::vec2 center, size_t count);
    bool IsPositionValid(std::unordered_set<glm::vec2> &existing, glm::vec2 p);
    glm::vec2 WorldToChunkSpace(const glm::vec3& pos);

private:
    std::map<MapPosition, std::shared_ptr<Chunk>> m_ChunkMap;
    std::unordered_set<std::shared_ptr<Chunk>> m_ChangedChunks;
    GLCore::Utils::PerspectiveCameraController &m_CameraController;

    siv::PerlinNoise m_Perlin;

    std::thread m_GenerationThread;
    std::shared_ptr<bool> m_ShouldGenerationRun;
    std::mutex m_Mutex;
};
}; // namespace Terrain
