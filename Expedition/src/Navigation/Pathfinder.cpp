#include "Pathfinder.hpp"

#include <algorithm>
#include <cmath>
#include <queue>
#include <unordered_set>

#include "Terrain/World/World.hpp"
#include "Terrain/Voxel/VoxelConstants.hpp"
#include "Config.hpp"

using namespace VoxelEngine;

namespace Expedition
{

Pathfinder::Pathfinder(World& world) : m_World(world) {}

// ── Voxel helpers ───────────────────────────────────────────────────

bool Pathfinder::IsSolid(int x, int y, int z) const
{
    if (y < 0 || y >= CHUNK_HEIGHT)
        return false;

    auto [chunkPos, voxelPos] = World::GlobalToWorldSpace(glm::i32vec3(x, y, z));
    const auto& chunkMap = m_World.GetChunkMap();
    auto it = chunkMap.find(chunkPos);
    if (it == chunkMap.end())
        return false;

    return it->second->GetVoxelFromGrid(voxelPos).GetVoxelType() != AIR;
}

bool Pathfinder::GetGroundHeight(float worldX, float worldZ, float searchY, float& outY) const
{
    const int ix = static_cast<int>(std::floor(worldX));
    const int iz = static_cast<int>(std::floor(worldZ));
    const int startY = static_cast<int>(searchY) + 5; // search a bit above current height
    const int minY = std::max(0, static_cast<int>(searchY) - 20);

    // Scan downward: find topmost solid voxel with air above
    for (int y = startY; y >= minY; --y)
    {
        if (IsSolid(ix, y, iz) && !IsSolid(ix, y + 1, iz))
        {
            outY = static_cast<float>(y + 1); // stand on top of this voxel
            return true;
        }
    }
    return false;
}

bool Pathfinder::IsBlocked(glm::vec3 from, glm::vec3 to) const
{
    // Walk along the XZ line and check for solid voxels at body height
    glm::vec3 dir(to.x - from.x, 0.0f, to.z - from.z);
    const float len = glm::length(dir);
    if (len < 0.01f) return false;

    const glm::vec3 step = dir / len * 0.5f; // sample every 0.5 units
    const int numSteps = static_cast<int>(len / 0.5f) + 1;

    const float groundY = from.y;

    for (int i = 1; i < numSteps; ++i)
    {
        const float px = from.x + step.x * static_cast<float>(i);
        const float pz = from.z + step.z * static_cast<float>(i);
        const int ix = static_cast<int>(std::floor(px));
        const int iz = static_cast<int>(std::floor(pz));
        const int iy = static_cast<int>(std::floor(groundY));

        // Check voxels at feet, waist, and head height
        for (int h = 0; h < EnemyHeight; ++h)
        {
            if (IsSolid(ix, iy + h, iz))
                return true;
        }
    }
    return false;
}

bool Pathfinder::IsCellWalkable(int cellX, int cellZ, float groundY) const
{
    const int iy = static_cast<int>(std::floor(groundY));

    // Check the cell and its neighbors within Padding radius
    for (int px = -Padding; px <= Padding; ++px)
    {
        for (int pz = -Padding; pz <= Padding; ++pz)
        {
            const int cx = cellX + px;
            const int cz = cellZ + pz;

            for (int h = 0; h < EnemyHeight; ++h)
            {
                if (IsSolid(cx, iy + h, cz))
                    return false;
            }
        }
    }
    return true;
}

// ── A* ──────────────────────────────────────────────────────────────

struct Node
{
    int   x, z;
    float groundY;
    float gCost = 0.0f;
    float hCost = 0.0f;
    float fCost() const { return gCost + hCost; }
    int   parentIdx = -1;
};

struct CompareF
{
    bool operator()(const std::pair<float, int>& a, const std::pair<float, int>& b) const
    {
        return a.first > b.first; // min-heap
    }
};

static int64_t CellKey(int x, int z)
{
    return (static_cast<int64_t>(x) << 32) | static_cast<int64_t>(static_cast<uint32_t>(z));
}

std::vector<glm::vec3> Pathfinder::FindPath(glm::vec3 start, glm::vec3 goal, float maxStepHeight)
{
    const int sx = static_cast<int>(std::floor(start.x / CellSize));
    const int sz = static_cast<int>(std::floor(start.z / CellSize));
    const int gx = static_cast<int>(std::floor(goal.x / CellSize));
    const int gz = static_cast<int>(std::floor(goal.z / CellSize));

    // Get ground height at start
    float startY = start.y;
    GetGroundHeight(static_cast<float>(sx) * CellSize + CellSize * 0.5f,
                    static_cast<float>(sz) * CellSize + CellSize * 0.5f,
                    start.y, startY);

    std::vector<Node> nodes;
    nodes.reserve(MaxSearch);
    std::unordered_set<int64_t> closed;
    std::priority_queue<std::pair<float, int>, std::vector<std::pair<float, int>>, CompareF> open;

    // Start node
    Node startNode;
    startNode.x       = sx;
    startNode.z       = sz;
    startNode.groundY = startY;
    startNode.gCost   = 0.0f;
    startNode.hCost   = glm::length(glm::vec2(
        static_cast<float>(gx - sx), static_cast<float>(gz - sz))) * CellSize;
    nodes.push_back(startNode);
    open.push({ startNode.fCost(), 0 });

    constexpr int dx[] = { 1, -1, 0, 0, 1, -1, 1, -1 };
    constexpr int dz[] = { 0, 0, 1, -1, 1, 1, -1, -1 };
    constexpr float dCost[] = { 1.0f, 1.0f, 1.0f, 1.0f, 1.414f, 1.414f, 1.414f, 1.414f };

    int iterations = 0;

    while (!open.empty() && iterations < MaxSearch)
    {
        const auto [topF, topIdx] = open.top();
        open.pop();

        const Node& current = nodes[topIdx];
        const int64_t curKey = CellKey(current.x, current.z);

        if (closed.count(curKey))
            continue;
        closed.insert(curKey);
        ++iterations;

        // Goal reached
        if (current.x == gx && current.z == gz)
        {
            // Reconstruct path
            std::vector<glm::vec3> path;
            int idx = topIdx;
            while (idx >= 0)
            {
                const Node& n = nodes[idx];
                path.push_back(glm::vec3(
                    static_cast<float>(n.x) * CellSize + CellSize * 0.5f,
                    n.groundY,
                    static_cast<float>(n.z) * CellSize + CellSize * 0.5f));
                idx = n.parentIdx;
            }
            std::reverse(path.begin(), path.end());
            return path;
        }

        // Expand neighbors
        for (int d = 0; d < 8; ++d)
        {
            const int nx = current.x + dx[d];
            const int nz = current.z + dz[d];

            if (closed.count(CellKey(nx, nz)))
                continue;

            float neighborY = 0.0f;
            const float worldX = static_cast<float>(nx) * CellSize + CellSize * 0.5f;
            const float worldZ = static_cast<float>(nz) * CellSize + CellSize * 0.5f;

            if (!GetGroundHeight(worldX, worldZ, current.groundY, neighborY))
                continue; // no ground — void/pit

            if (std::abs(neighborY - current.groundY) > maxStepHeight)
                continue; // too steep

            // Check the cell is walkable (accounts for enemy body radius)
            if (!IsCellWalkable(nx, nz, neighborY))
                continue;

            const float newG = current.gCost + dCost[d] * CellSize;
            const float h = glm::length(glm::vec2(
                static_cast<float>(gx - nx), static_cast<float>(gz - nz))) * CellSize;

            Node neighbor;
            neighbor.x         = nx;
            neighbor.z         = nz;
            neighbor.groundY   = neighborY;
            neighbor.gCost     = newG;
            neighbor.hCost     = h;
            neighbor.parentIdx = topIdx;

            const int newIdx = static_cast<int>(nodes.size());
            nodes.push_back(neighbor);
            open.push({ neighbor.fCost(), newIdx });
        }
    }

    return {}; // no path found
}

}
