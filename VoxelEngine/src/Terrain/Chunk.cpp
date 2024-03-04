#include "Chunk.hpp"
#include "GLCoreUtils.hpp"
#include "VoxelMeshBuilder.hpp"

namespace Terrain
{
Chunk::Chunk(const siv::PerlinNoise &perlin) : Chunk(glm::vec3(0), perlin)
{
}

Chunk::Chunk(glm::vec3 position, const siv::PerlinNoise &perlin)
    : m_Position(position), m_Mesh({}), m_VoxelGrid({}), m_Perlin(perlin)
{
}

Chunk::~Chunk()
{
}

void Chunk::Generate()
{
    m_VoxelGrid = {};
    for (size_t x = 0; x < CHUNK_WIDTH; ++x)
    {
        m_VoxelGrid.push_back({});
        for (size_t z = 0; z < CHUNK_WIDTH; ++z)
        {
            m_VoxelGrid[x].push_back({});
            const double noise = m_Perlin.octave2D_01((m_Position.x * CHUNK_WIDTH + x) * 0.02,
                                                      (m_Position.z * CHUNK_WIDTH + z) * 0.02,
                                                      4);
            for (size_t y = 0; y < CHUNK_HEIGHT; ++y)
            {
                VoxelType type = VoxelType::STONE;
                size_t height = (size_t)(noise * CHUNK_HEIGHT);
                if (y > height)
                    type = VoxelType::AIR;
                else if (y > height - 1)
                    type = VoxelType::GRASS;
                m_VoxelGrid[x][z].push_back(Voxel(type, glm::vec3(x, y, z)));
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
        for (size_t z = 0; z < CHUNK_WIDTH; ++z)
        {
            for (size_t y = 0; y < CHUNK_HEIGHT; ++y)
            {
                Voxel &v = m_VoxelGrid[x][z][y];
                if (v.GetVoxelType() == VoxelType::AIR)
                    continue;

                if (y != 0 && m_VoxelGrid[x][z][y - 1].GetVoxelType() == VoxelType::AIR)
                    v.SetFaceVisible(VoxelFace::BOTTOM, true);
                if (y == CHUNK_HEIGHT - 1 || m_VoxelGrid[x][z][y + 1].GetVoxelType() == VoxelType::AIR)
                    v.SetFaceVisible(VoxelFace::TOP, true);

                if (z < CHUNK_WIDTH - 1 && m_VoxelGrid[x][z + 1][y].GetVoxelType() == VoxelType::AIR)
                    v.SetFaceVisible(VoxelFace::FRONT, true);
                if (z > 0 && m_VoxelGrid[x][z - 1][y].GetVoxelType() == VoxelType::AIR)
                    v.SetFaceVisible(VoxelFace::BACK, true);

                if (x < CHUNK_WIDTH - 1 && m_VoxelGrid[x + 1][z][y].GetVoxelType() == VoxelType::AIR)
                    v.SetFaceVisible(VoxelFace::RIGHT, true);
                if (x > 0 && m_VoxelGrid[x - 1][z][y].GetVoxelType() == VoxelType::AIR)
                    v.SetFaceVisible(VoxelFace::LEFT, true);

                std::vector<Vertex> data = meshBuilder.FromVoxel(v);
                m_Mesh.insert(m_Mesh.begin(), data.begin(), data.end());
            }
        }
    }
}

glm::mat4 Chunk::GetModelMatrix() const
{
    glm::mat4 model(1.0f);
    glm::vec3 pos = m_Position;
    pos.x *= CHUNK_WIDTH;
    pos.y *= CHUNK_HEIGHT;
    pos.z *= CHUNK_WIDTH;
    return glm::translate(model, pos);
}
}; // namespace Terrain
