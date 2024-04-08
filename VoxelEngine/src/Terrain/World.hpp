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


namespace Terrain
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
    void CheckChunkEdges(Chunk &chunk, Neighbours &neighbours);
    void CheckVoxelEdge(Voxel &v1, Voxel &v2, VoxelFace face);
    Neighbours GetNeighbours(Chunk &chunk);

    void GenerateWorld();
    void GenerateChunk(Position2D position);
    std::queue<Position2D> FindNextChunkLocations(glm::vec2 center, size_t count);
    bool IsPositionValid(std::unordered_set<glm::vec2> &existing, glm::vec2 p);
    glm::vec2 WorldToChunkSpace(const glm::vec3 &pos);

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
