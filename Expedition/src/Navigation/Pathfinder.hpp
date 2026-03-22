#pragma once

#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>

namespace VoxelEngine { class World; class Chunk; }

namespace Expedition
{

class Pathfinder
{
public:
    explicit Pathfinder(VoxelEngine::World& world);

    // Returns world-space waypoints from start to goal. Empty = no path found.
    std::vector<glm::vec3> FindPath(glm::vec3 start, glm::vec3 goal, float maxStepHeight = 1.5f);

private:
    bool GetGroundHeight(float worldX, float worldZ, float searchY, float& outY);
    bool IsSolid(int x, int y, int z);
    bool IsCellWalkable(int cellX, int cellZ, float groundY);

    // Cached chunk lookup — avoids repeated hash map finds
    VoxelEngine::Chunk* GetChunk(int chunkX, int chunkZ);

    VoxelEngine::World& m_World;

    // Per-pathfind caches (cleared each FindPath call)
    std::unordered_map<int64_t, VoxelEngine::Chunk*> m_ChunkCache;
    std::unordered_map<int64_t, bool>                m_SolidCache;

    static constexpr float CellSize    = 1.0f;
    static constexpr int   MaxSearch   = 1200;
    static constexpr int   EnemyHeight = 4;
    static constexpr int   Padding     = 1;
};

}
