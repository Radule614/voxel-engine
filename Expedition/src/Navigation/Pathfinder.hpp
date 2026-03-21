#pragma once

#include <vector>
#include <glm/glm.hpp>

namespace VoxelEngine { class World; }

namespace Expedition
{

class Pathfinder
{
public:
    explicit Pathfinder(VoxelEngine::World& world);

    // Returns world-space waypoints from start to goal. Empty = no path found.
    std::vector<glm::vec3> FindPath(glm::vec3 start, glm::vec3 goal, float maxStepHeight = 1.5f);

private:
    // Query voxel grid for ground height at (worldX, worldZ).
    // Scans downward from searchY to find the topmost solid voxel with air above.
    bool GetGroundHeight(float worldX, float worldZ, float searchY, float& outY) const;

    // Check if path between two ground positions is blocked by solid voxels
    bool IsBlocked(glm::vec3 from, glm::vec3 to) const;

    // Check if a voxel at global position is solid (non-AIR)
    bool IsSolid(int x, int y, int z) const;

    VoxelEngine::World& m_World;

    static constexpr float CellSize  = 1.0f;
    static constexpr int   MaxSearch = 500;
    static constexpr int   EnemyHeight = 4;  // check this many voxels above ground for clearance
};

}
