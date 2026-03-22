#pragma once

#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>

namespace VoxelEngine { class World; class Chunk; }

namespace Expedition
{

struct Waypoint
{
    glm::vec3 position;
};

class Pathfinder
{
public:
    explicit Pathfinder(VoxelEngine::World& world);

    // Returns world-space waypoints from start to goal. Empty = no path found.
    std::vector<Waypoint> FindPath(glm::vec3 start, glm::vec3 goal);

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

    static constexpr float CellSize        = 1.0f;
    static constexpr int   MaxSearch       = 3000;
    static constexpr int   EnemyHeight     = 3;
    static constexpr int   Padding         = 1;     // capsule radius 0.75 needs clearance
    static constexpr float MaxClimbHeight  = 2.0f;  // can climb up to 2 voxels high
};

}
