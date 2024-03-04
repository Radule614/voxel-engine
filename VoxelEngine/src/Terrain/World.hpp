#pragma once

#include <map>
#include <glm/glm.hpp>

#include "Chunk.hpp"
#include "MapPosition.hpp"

namespace Terrain
{
class World
{
public:
    World();
    ~World();

    const std::map<MapPosition, Chunk> &GetChunkMap() const;
    void Generate();

private:
    void CheckChunkEdges(Chunk &chunk);
    void CheckVoxelEdge(Voxel& v1, Voxel& v2, VoxelFace face);

private:
    std::map<MapPosition, Chunk> m_ChunkMap;
};
}; // namespace Terrain
