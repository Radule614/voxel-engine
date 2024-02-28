#pragma once

#include <vector>

class ChunkMeshGenerator
{
public:
    ChunkMeshGenerator();
    ~ChunkMeshGenerator();

private:
    std::vector<float> m_Mesh;
};
