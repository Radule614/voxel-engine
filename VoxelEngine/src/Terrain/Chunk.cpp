#include "Chunk.hpp"
#include "GLCoreUtils.hpp"
#include "VoxelMeshBuilder.hpp"

namespace Terrain
{
Chunk::Chunk() : Chunk(glm::vec3(0))
{
}

Chunk::Chunk(glm::vec3 position) : m_Position(position), m_Mesh({}), m_VoxelGrid({})
{
    Init();
}

Chunk::~Chunk()
{
}

void Chunk::Init()
{
    m_VoxelGrid = {};
    size_t height = 44 + ((int)m_Position.x * (int)m_Position.z) % 24;
    for (size_t x = 0; x < CHUNK_WIDTH; ++x)
    {
        m_VoxelGrid.push_back({});
        for (size_t y = 0; y < CHUNK_HEIGHT; ++y)
        {
            m_VoxelGrid[x].push_back({});
            for (size_t z = 0; z < CHUNK_WIDTH; ++z)
            {
                VoxelType type = VoxelType::GRASS;
                if (y > height)
                    type = VoxelType::AIR;
                m_VoxelGrid[x][y].push_back(Voxel(type, glm::vec3(x, y, z)));
            }
        }
    }
}

void Chunk::GenerateMesh()
{
    m_Mesh = {};
    VoxelMeshBuilder meshBuilder;
    for (size_t x = 0; x < CHUNK_WIDTH; ++x)
    {
        for (size_t y = 0; y < CHUNK_HEIGHT; ++y)
        {
            for (size_t z = 0; z < CHUNK_WIDTH; ++z)
            {
                Voxel &v = m_VoxelGrid[x][y][z];
                if (v.GetVoxelType() == VoxelType::AIR)
                    continue;

                if (y == 0 || m_VoxelGrid[x][y - 1][z].GetVoxelType() == VoxelType::AIR)
                    v.SetFaceVisible(VoxelFace::BOTTOM, true);
                if (y == CHUNK_HEIGHT - 1 || m_VoxelGrid[x][y + 1][z].GetVoxelType() == VoxelType::AIR)
                    v.SetFaceVisible(VoxelFace::TOP, true);

                if (z < CHUNK_WIDTH - 1 && m_VoxelGrid[x][y][z + 1].GetVoxelType() == VoxelType::AIR)
                    v.SetFaceVisible(VoxelFace::FRONT, true);
                if (z > 0 && m_VoxelGrid[x][y][z - 1].GetVoxelType() == VoxelType::AIR)
                    v.SetFaceVisible(VoxelFace::BACK, true);

                if (x < CHUNK_WIDTH - 1 && m_VoxelGrid[x + 1][y][z].GetVoxelType() == VoxelType::AIR)
                    v.SetFaceVisible(VoxelFace::RIGHT, true);
                if (x > 0 && m_VoxelGrid[x - 1][y][z].GetVoxelType() == VoxelType::AIR)
                    v.SetFaceVisible(VoxelFace::LEFT, true);

                std::vector<Vertex> data = meshBuilder.FromVoxel(v);
                m_Mesh.insert(m_Mesh.begin(), data.begin(), data.end());
            }
        }
    }
}
}; // namespace Terrain
