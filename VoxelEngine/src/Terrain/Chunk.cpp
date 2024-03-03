#include "Chunk.hpp"

#include "GLCoreUtils.hpp"

#include "VoxelMeshBuilder.hpp"

namespace Terrain
{
Chunk::Chunk() : Chunk(glm::vec3(0))
{
}

Chunk::Chunk(glm::vec3 position) : m_Position(position), m_Mesh({}), m_VoxelMap({})
{
    Init();
}

Chunk::~Chunk()
{
}

void Chunk::Init()
{
    std::vector<std::vector<std::vector<Voxel>>> voxelGrid = {};
    for (size_t y = 0; y < CHUNK_HEIGHT; ++y)
    {
        voxelGrid.push_back({});
        for (size_t z = 0; z < CHUNK_WIDTH; ++z)
        {
            voxelGrid[y].push_back({});
            for (size_t x = 0; x < CHUNK_WIDTH; ++x)
            {
                voxelGrid[y][z].push_back(Voxel(VoxelType::GRASS, glm::vec3(x, y, z)));
            }
        }
    }
    GenerateMesh(voxelGrid);
}

void Chunk::GenerateMesh(std::vector<std::vector<std::vector<Voxel>>> &voxelGrid)
{
    m_Mesh = {};
    VoxelMeshBuilder meshBuilder;
    for (size_t y = 0; y < CHUNK_HEIGHT; ++y)
    {
        for (size_t z = 0; z < CHUNK_WIDTH; ++z)
        {
            for (size_t x = 0; x < CHUNK_WIDTH; ++x)
            {
                Voxel &v = voxelGrid[y][z][x];
                if (v.GetVoxelType() == VoxelType::AIR)
                    continue;

                if (y == 0 || voxelGrid[y - 1][z][x].GetVoxelType() == VoxelType::AIR)
                    v.SetFaceVisible(VoxelFace::BOTTOM, true);
                if (y == CHUNK_HEIGHT - 1 || voxelGrid[y + 1][z][x].GetVoxelType() == VoxelType::AIR)
                    v.SetFaceVisible(VoxelFace::TOP, true);
                if (z == 0 || voxelGrid[y][z - 1][x].GetVoxelType() == VoxelType::AIR)
                    v.SetFaceVisible(VoxelFace::BACK, true);
                if (z == CHUNK_WIDTH - 1 || voxelGrid[y][z + 1][x].GetVoxelType() == VoxelType::AIR)
                    v.SetFaceVisible(VoxelFace::FRONT, true);
                if (x == 0 || voxelGrid[y][z][x - 1].GetVoxelType() == VoxelType::AIR)
                    v.SetFaceVisible(VoxelFace::LEFT, true);
                if (x == CHUNK_WIDTH - 1 || voxelGrid[y][z][x + 1].GetVoxelType() == VoxelType::AIR)
                    v.SetFaceVisible(VoxelFace::RIGHT, true);

                std::vector<Vertex> data = meshBuilder.FromVoxel(v);
                m_Mesh.insert(m_Mesh.begin(), data.begin(), data.end());

                VoxelPosition pos(v.GetPosition());
                m_VoxelMap.insert({pos, v});
            }
        }
    }
}
}; // namespace Terrain
