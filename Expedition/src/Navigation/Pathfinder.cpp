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

// ── Cached chunk lookup ─────────────────────────────────────────────

Chunk* Pathfinder::GetChunk(int chunkX, int chunkZ)
{
    const int64_t key = (static_cast<int64_t>(chunkX) << 32) |
                         static_cast<int64_t>(static_cast<uint32_t>(chunkZ));
    auto it = m_ChunkCache.find(key);
    if (it != m_ChunkCache.end())
        return it->second;

    const Position2D pos(static_cast<int16_t>(chunkX), static_cast<int16_t>(chunkZ));
    const auto& chunkMap = m_World.GetChunkMap();
    auto mapIt = chunkMap.find(pos);
    Chunk* chunk = (mapIt != chunkMap.end()) ? mapIt->second.get() : nullptr;
    m_ChunkCache[key] = chunk;
    return chunk;
}

// ── Voxel helpers (cached) ──────────────────────────────────────────

bool Pathfinder::IsSolid(int x, int y, int z)
{
    if (y < 0 || y >= CHUNK_HEIGHT)
        return false;

    const int64_t key = (static_cast<int64_t>(x & 0x1FFFFF) << 29) |
                        (static_cast<int64_t>(y & 0xFF) << 21) |
                         static_cast<int64_t>(z & 0x1FFFFF);
    auto cacheIt = m_SolidCache.find(key);
    if (cacheIt != m_SolidCache.end())
        return cacheIt->second;

    const int chunkX = (x >= 0) ? (x / CHUNK_WIDTH) : ((x + 1) / CHUNK_WIDTH - 1);
    const int chunkZ = (z >= 0) ? (z / CHUNK_WIDTH) : ((z + 1) / CHUNK_WIDTH - 1);

    Chunk* chunk = GetChunk(chunkX, chunkZ);
    if (!chunk)
    {
        m_SolidCache[key] = false;
        return false;
    }

    int lx = x % CHUNK_WIDTH;
    int lz = z % CHUNK_WIDTH;
    if (lx < 0) lx += CHUNK_WIDTH;
    if (lz < 0) lz += CHUNK_WIDTH;

    const bool solid = chunk->GetVoxelFromGrid(
        Position3D(glm::i32vec3(lx, y, lz))).GetVoxelType() != AIR;

    m_SolidCache[key] = solid;
    return solid;
}

bool Pathfinder::GetGroundHeight(float worldX, float worldZ, float searchY, float& outY)
{
    const int ix = static_cast<int>(std::floor(worldX));
    const int iz = static_cast<int>(std::floor(worldZ));
    const int sy = static_cast<int>(searchY);

    // Scan ±15 around searchY to handle falling enemies / terrain variation.
    // "Closest to searchY" logic still avoids finding tree canopy tops as ground.
    const int lo = std::max(0, sy - 15);
    const int hi = std::min(CHUNK_HEIGHT - 2, sy + 15);

    int   bestY    = -1;
    int   bestDist = 999;

    for (int y = lo; y <= hi; ++y)
    {
        if (IsSolid(ix, y, iz) && !IsSolid(ix, y + 1, iz))
        {
            const int dist = std::abs((y + 1) - sy);
            if (dist < bestDist)
            {
                bestDist = dist;
                bestY    = y + 1;
            }
        }
    }

    if (bestY >= 0)
    {
        outY = static_cast<float>(bestY);
        return true;
    }
    return false;
}

bool Pathfinder::IsCellWalkable(int cellX, int cellZ, float groundY)
{
    const int iy = static_cast<int>(std::floor(groundY));

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
    bool  needsJump = false;  // transition to this node requires a jump
};

struct CompareF
{
    bool operator()(const std::pair<float, int>& a, const std::pair<float, int>& b) const
    {
        return a.first > b.first;
    }
};

static int64_t CellKey(int x, int z)
{
    return (static_cast<int64_t>(x) << 32) | static_cast<int64_t>(static_cast<uint32_t>(z));
}

std::vector<Waypoint> Pathfinder::FindPath(glm::vec3 start, glm::vec3 goal)
{
    m_ChunkCache.clear();
    m_SolidCache.clear();

    const int sx = static_cast<int>(std::floor(start.x / CellSize));
    const int sz = static_cast<int>(std::floor(start.z / CellSize));
    const int gx = static_cast<int>(std::floor(goal.x / CellSize));
    const int gz = static_cast<int>(std::floor(goal.z / CellSize));

    float startY = start.y;
    GetGroundHeight(static_cast<float>(sx) * CellSize + CellSize * 0.5f,
                    static_cast<float>(sz) * CellSize + CellSize * 0.5f,
                    start.y, startY);

    std::vector<Node> nodes;
    nodes.reserve(MaxSearch);

    std::unordered_map<int64_t, float> bestG;
    std::unordered_set<int64_t> closed;
    std::priority_queue<std::pair<float, int>, std::vector<std::pair<float, int>>, CompareF> open;

    Node startNode;
    startNode.x       = sx;
    startNode.z       = sz;
    startNode.groundY = startY;
    startNode.gCost   = 0.0f;
    startNode.hCost   = glm::length(glm::vec2(
        static_cast<float>(gx - sx), static_cast<float>(gz - sz))) * CellSize;
    nodes.push_back(startNode);
    open.push({ startNode.fCost(), 0 });
    bestG[CellKey(sx, sz)] = 0.0f;

    constexpr int dx[] = { 1, -1, 0, 0, 1, -1, 1, -1 };
    constexpr int dz[] = { 0, 0, 1, -1, 1, 1, -1, -1 };
    constexpr float dCost[] = { 1.0f, 1.0f, 1.0f, 1.0f, 1.414f, 1.414f, 1.414f, 1.414f };

    int iterations = 0;
    int bestIdx = 0;           // track node closest to goal
    float bestHeuristic = startNode.hCost;

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

        // Track the closest node to goal (for partial path fallback)
        if (current.hCost < bestHeuristic)
        {
            bestHeuristic = current.hCost;
            bestIdx = topIdx;
        }

        // Goal reached
        if (current.x == gx && current.z == gz)
        {
            std::vector<Waypoint> path;
            int idx = topIdx;
            while (idx >= 0)
            {
                const Node& n = nodes[idx];
                path.push_back({
                    glm::vec3(
                        static_cast<float>(n.x) * CellSize + CellSize * 0.5f,
                        n.groundY,
                        static_cast<float>(n.z) * CellSize + CellSize * 0.5f),
                    n.needsJump
                });
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
            const int64_t nKey = CellKey(nx, nz);

            if (closed.count(nKey))
                continue;

            float neighborY = 0.0f;
            const float worldX = static_cast<float>(nx) * CellSize + CellSize * 0.5f;
            const float worldZ = static_cast<float>(nz) * CellSize + CellSize * 0.5f;

            if (!GetGroundHeight(worldX, worldZ, current.groundY, neighborY))
                continue;

            const float heightDiff = neighborY - current.groundY;  // positive = uphill
            const float absHeightDiff = std::abs(heightDiff);

            if (absHeightDiff > MaxJumpStep)
                continue;

            // Only uphill steps need a jump — the capsule can fall down fine
            const bool requiresJump = heightDiff > MaxWalkStep;

            // Prevent diagonal corner-cutting
            if (d >= 4)
            {
                if (!IsCellWalkable(current.x + dx[d], current.z, current.groundY) ||
                    !IsCellWalkable(current.x, current.z + dz[d], current.groundY))
                    continue;
            }

            if (!IsCellWalkable(nx, nz, neighborY))
                continue;

            // Penalize jump transitions so flat paths are preferred
            const float newG = current.gCost + dCost[d] * CellSize;
            const float heightPenalty = requiresJump ? heightDiff * 3.0f : 0.0f;
            const float totalG = newG + heightPenalty;

            auto bestIt = bestG.find(nKey);
            if (bestIt != bestG.end() && totalG >= bestIt->second)
                continue;

            const float h = glm::length(glm::vec2(
                static_cast<float>(gx - nx), static_cast<float>(gz - nz))) * CellSize;

            Node neighbor;
            neighbor.x         = nx;
            neighbor.z         = nz;
            neighbor.groundY   = neighborY;
            neighbor.gCost     = totalG;
            neighbor.hCost     = h;
            neighbor.parentIdx = topIdx;
            neighbor.needsJump = requiresJump;

            bestG[nKey] = totalG;

            const int newIdx = static_cast<int>(nodes.size());
            nodes.push_back(neighbor);
            open.push({ neighbor.fCost(), newIdx });
        }
    }

    // No complete path found — return partial path to closest explored node
    if (bestIdx > 0 && bestHeuristic < startNode.hCost)
    {
        std::vector<Waypoint> path;
        int idx = bestIdx;
        while (idx >= 0)
        {
            const Node& n = nodes[idx];
            path.push_back({
                glm::vec3(
                    static_cast<float>(n.x) * CellSize + CellSize * 0.5f,
                    n.groundY,
                    static_cast<float>(n.z) * CellSize + CellSize * 0.5f),
                n.needsJump
            });
            idx = n.parentIdx;
        }
        std::reverse(path.begin(), path.end());
        return path;
    }

    return {};
}

}
