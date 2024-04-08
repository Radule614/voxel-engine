#pragma once
#include "Voxel.hpp"
#include "Chunk.hpp"

#include <map>

namespace Terrain
{
class Obstacle
{
public:
    Obstacle(Position3D root) : m_RootPosition(root)
    {
    }

    void Insert(std::shared_ptr<Chunk> chunk);
    virtual std::vector<Voxel> GetVoxels() = 0;

protected:
    Position3D m_RootPosition;
};
} // namespace Terrain
