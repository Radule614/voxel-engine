#include "Pathfinder.hpp"

#include <algorithm>
#include <cmath>
#include <queue>
#include <unordered_set>

#include "Jolt/Jolt.h"
#include "Jolt/Physics/Collision/RayCast.h"
#include "Jolt/Physics/Collision/CastResult.h"
#include "Jolt/Physics/Collision/NarrowPhaseQuery.h"
#include "Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h"

#include "Physics/PhysicsEngine.hpp"
#include "Physics/PhysicsEngineLayers.hpp"

using namespace VoxelEngine;

namespace Expedition
{

// ── Ray helpers ─────────────────────────────────────────────────────

bool Pathfinder::GetGroundHeight(float worldX, float worldZ, float& outY) const
{
    auto& system = PhysicsEngine::Instance().GetSystem();
    JPH::RRayCast ray(
        JPH::RVec3(worldX, RayStartY, worldZ),
        JPH::Vec3(0.0f, -RayLength, 0.0f));

    JPH::RayCastResult hit;
    JPH::SpecifiedBroadPhaseLayerFilter bpFilter(BroadPhaseLayers::NON_MOVING);
    if (system.GetNarrowPhaseQuery().CastRay(ray, hit, bpFilter))
    {
        outY = RayStartY + hit.mFraction * (-RayLength);
        return true;
    }
    return false;
}

bool Pathfinder::IsBlocked(glm::vec3 from, glm::vec3 to) const
{
    auto& system = PhysicsEngine::Instance().GetSystem();
    glm::vec3 dir = to - from;
    if (glm::length(dir) < 0.01f) return false;

    // Cast at waist height above ground
    JPH::RRayCast ray(
        JPH::RVec3(from.x, from.y + 1.0f, from.z),
        JPH::Vec3(dir.x, dir.y, dir.z));

    JPH::RayCastResult hit;
    JPH::SpecifiedBroadPhaseLayerFilter bpFilter(BroadPhaseLayers::NON_MOVING);
    if (system.GetNarrowPhaseQuery().CastRay(ray, hit, bpFilter))
        return hit.mFraction < 1.0f;
    return false;
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
                    startY);

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

            // Simple path smoothing: skip waypoints with line-of-sight
            if (path.size() > 2)
            {
                std::vector<glm::vec3> smoothed;
                smoothed.push_back(path[0]);
                size_t anchor = 0;
                for (size_t i = 2; i < path.size(); ++i)
                {
                    if (IsBlocked(path[anchor], path[i]))
                    {
                        smoothed.push_back(path[i - 1]);
                        anchor = i - 1;
                    }
                }
                smoothed.push_back(path.back());
                return smoothed;
            }
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

            if (!GetGroundHeight(worldX, worldZ, neighborY))
                continue; // no ground — void/pit

            if (std::abs(neighborY - current.groundY) > maxStepHeight)
                continue; // too steep

            const glm::vec3 curWorld(
                static_cast<float>(current.x) * CellSize + CellSize * 0.5f,
                current.groundY,
                static_cast<float>(current.z) * CellSize + CellSize * 0.5f);
            const glm::vec3 neighborWorld(worldX, neighborY, worldZ);

            if (IsBlocked(curWorld, neighborWorld))
                continue; // wall in between

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
