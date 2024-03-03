#pragma once

#include <vector>

namespace Terrain
{
class ChunkMeshGenerator
{
public:
    ChunkMeshGenerator();
    ~ChunkMeshGenerator();

private:
    std::vector<float> m_Mesh;
};
}; // namespace Terrain
