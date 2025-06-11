#include "Chunk.hpp"
#include "GLCoreUtils.hpp"
#include "VoxelMeshBuilder.hpp"
#include "Position2D.hpp"
#include "../Utils/Utils.hpp"
#include "World.hpp"
#include "Tree.hpp"

#include <execution>

namespace VoxelEngine
{

Chunk::Chunk(World& world, const siv::PerlinNoise& perlin) : Chunk(world, Position2D(), perlin)
{
}

Chunk::Chunk(World& world, Position2D position, const siv::PerlinNoise& perlin)
    : m_World(world), m_Position(position), m_Mesh({}), m_VoxelGrid(), m_Perlin(perlin), m_Mutex(std::mutex())
{
    m_BorderMeshes.insert({FRONT, {}});
    m_BorderMeshes.insert({RIGHT, {}});
    m_BorderMeshes.insert({BACK, {}});
    m_BorderMeshes.insert({LEFT, {}});
}

Chunk::~Chunk() = default;

void Chunk::Generate()
{
    size_t heightMap[CHUNK_WIDTH][CHUNK_WIDTH]{};
    for (size_t x = 0; x < CHUNK_WIDTH; ++x)
    {
        for (size_t z = 0; z < CHUNK_WIDTH; ++z)
        {
            const double_t height_bias = m_Perlin.octave2D_01(
                (x + static_cast<double_t>(m_Position.x) * CHUNK_WIDTH) * 0.01,
                (z + static_cast<double_t>(m_Position.y) * CHUNK_WIDTH) * 0.01,
                8);
            size_t h = CHUNK_HEIGHT / 5 + glm::floor(height_bias * 3 * CHUNK_HEIGHT / 5);
            heightMap[x][z] = h;
            std::for_each(std::execution::par,
                          std::begin(m_VoxelGrid[x][z]),
                          std::end(m_VoxelGrid[x][z]),
                          [&](Voxel& v) { this->DetermineVoxelFeatures(v, x, z, h); });
        }
    }

    int32_t i = 0;
    std::vector<Structure> structures{};
    double_t treeChance = m_Perlin.octave2D_01(static_cast<double_t>(m_Position.x) + i,
                                               static_cast<double_t>(m_Position.y) + i,
                                               2);
    while (treeChance > 0.55 && structures.size() < 2)
    {
        const int32_t random = m_Perlin.octave2D_01(static_cast<double_t>(m_Position.x) + i,
                                                    static_cast<double_t>(m_Position.y) + i,
                                                    2) * CHUNK_WIDTH * CHUNK_WIDTH;
        size_t x = random / CHUNK_WIDTH;
        size_t z = random % CHUNK_WIDTH;
        ++i;
        if (!InRange(x, 0, CHUNK_WIDTH - 1) || !InRange(z, 0, CHUNK_WIDTH - 1))
            continue;
        bool isValid = true;
        for (auto& s: structures)
        {
            Position2D p(s.GetRoot().GetPosition().GetX() - x, s.GetRoot().GetPosition().GetZ() - z);
            if (p.GetLength() < s.GetRadius())
            {
                isValid = false;
                break;
            }
        }
        if (isValid)
        {
            if (treeChance > 0.7)
                structures.push_back(LargeTree(Position3D(x, heightMap[x][z], z)));
            else
                structures.push_back(Tree(Position3D(x, heightMap[x][z], z)));
        }
    }
    AddStructures(structures);
}

void Chunk::AddStructures(std::vector<Structure> structures)
{
    std::unordered_set<std::shared_ptr<Chunk> > changedChunks{};
    auto& deferredQueueMap = m_World.GetDeferredChunkQueue();
    for (auto& s: structures)
    {
        Position3D p = s.GetRoot().GetPosition();
        VoxelType soilType = m_VoxelGrid[p.GetX()][p.GetZ()][p.y - 1].GetVoxelType();
        if (soilType == AIR || soilType == SNOW)
            continue;
        m_VoxelGrid[p.GetX()][p.GetZ()][p.y].SetVoxelType(s.GetRoot().GetVoxelType());
        m_VoxelGrid[p.GetX()][p.GetZ()][p.y].SetPosition(p);
        for (auto& v: s.GetVoxelData())
        {
            glm::i16vec3 position = static_cast<glm::i16vec3>(p) + v.first;
            auto [chunkPosition, voxelPosition] = GetPositionRelativeToWorld(position);
            if (chunkPosition == m_Position)
            {
                Voxel& voxel = m_VoxelGrid[voxelPosition.GetX()][voxelPosition.GetZ()][voxelPosition.y];
                voxel.SetVoxelType(v.second);
                voxel.SetPosition(voxelPosition);
                continue;
            }
            if (m_World.GetChunkMap().find(chunkPosition) != m_World.GetChunkMap().end())
            {
                auto& chunk = m_World.GetChunkMap().at(chunkPosition);
                chunk->GetLock().lock();
                auto& voxelGrid = chunk->GetVoxelGrid();
                Voxel& voxel = voxelGrid[voxelPosition.GetX()][voxelPosition.GetZ()][voxelPosition.y];
                voxel.SetVoxelType(v.second);
                voxel.SetPosition(voxelPosition);
                changedChunks.insert(chunk);
                chunk->GetLock().unlock();
                continue;
            }
            deferredQueueMap[chunkPosition].emplace(v.second, voxelPosition);
        }
    }
    m_World.GetLock().lock();
    for (auto& c: changedChunks)
    {
        c->GetLock().lock();
        c->GenerateMesh();
        m_World.GetChangedChunks().insert(c);
        c->GetLock().unlock();
    }
    m_World.GetLock().unlock();
}

void Chunk::GenerateMesh()
{
    m_Mesh.clear();
    VoxelMeshBuilder meshBuilder;
    for (size_t x = 0; x < CHUNK_WIDTH; ++x)
    {
        for (size_t z = 0; z < CHUNK_WIDTH; ++z)
        {
            for (size_t y = 0; y < CHUNK_HEIGHT; ++y)
            {
                Voxel v = m_VoxelGrid[x][z][y];
                if (v.GetVoxelType() == AIR)
                    continue;

                if (y != 0 && m_VoxelGrid[x][z][y - 1].IsTransparent())
                    v.SetFaceVisible(BOTTOM, true);
                if (y == CHUNK_HEIGHT - 1 || m_VoxelGrid[x][z][y + 1].IsTransparent())
                    v.SetFaceVisible(TOP, true);

                if (z < CHUNK_WIDTH - 1 && m_VoxelGrid[x][z + 1][y].IsTransparent())
                    v.SetFaceVisible(FRONT, true);
                if (z > 0 && m_VoxelGrid[x][z - 1][y].IsTransparent())
                    v.SetFaceVisible(BACK, true);

                if (x < CHUNK_WIDTH - 1 && m_VoxelGrid[x + 1][z][y].IsTransparent())
                    v.SetFaceVisible(RIGHT, true);
                if (x > 0 && m_VoxelGrid[x - 1][z][y].IsTransparent())
                    v.SetFaceVisible(LEFT, true);

                if (x == 0 || z == 0 || x == CHUNK_WIDTH - 1 || z == CHUNK_WIDTH - 1)
                    DetermineEdgeMeshes(meshBuilder, v, x, z);
                else
                {
                    std::vector<VoxelVertex> data = meshBuilder.FromVoxel(v);
                    m_Mesh.insert(m_Mesh.begin(), data.begin(), data.end());
                }
            }
        }
    }
}

void Chunk::GenerateEdgeMesh(const VoxelFace face)
{
    std::vector<VoxelVertex>& mesh = m_BorderMeshes.at(face);
    mesh.clear();
    VoxelMeshBuilder meshBuilder;
    for (size_t x = 0; x < CHUNK_WIDTH; x++)
    {
        for (size_t y = 0; y < CHUNK_HEIGHT; y++)
        {
            Voxel v = m_VoxelGrid[x][x][y];
            if (face == FRONT)
                v = m_VoxelGrid[x][CHUNK_WIDTH - 1][y];
            else if (face == BACK)
                v = m_VoxelGrid[x][0][y];
            else if (face == RIGHT)
                v = m_VoxelGrid[CHUNK_WIDTH - 1][x][y];
            else if (face == LEFT)
                v = m_VoxelGrid[0][x][y];
            if (!v.IsFaceVisible(face))
                continue;
            std::vector<VoxelVertex> data = meshBuilder.FromVoxel(v, face);
            mesh.insert(mesh.end(), data.begin(), data.end());
        }
    }
}

std::pair<Position2D, Position3D> Chunk::GetPositionRelativeToWorld(glm::i16vec3 pos) const
{
    if (InRange(pos.x, 0, CHUNK_WIDTH - 1) && InRange(pos.y, 0, CHUNK_WIDTH - 1) && InRange(pos.z, 0, CHUNK_WIDTH - 1))
        return {m_Position, Position3D(pos.x, pos.y, pos.z)};
    Position2D chunkPos = m_Position;
    while (pos.x < 0)
    {
        pos.x += CHUNK_WIDTH;
        --chunkPos.x;
    }
    while (pos.x > CHUNK_WIDTH - 1)
    {
        pos.x -= CHUNK_WIDTH;
        ++chunkPos.x;
    }
    while (pos.z < 0)
    {
        pos.z += CHUNK_WIDTH;
        --chunkPos.y;
    }
    while (pos.z > CHUNK_WIDTH - 1)
    {
        pos.z -= CHUNK_WIDTH;
        ++chunkPos.y;
    }
    return {chunkPos, Position3D(pos.x, pos.y, pos.z)};
}

void Chunk::DetermineEdgeMeshes(VoxelMeshBuilder& meshBuilder, Voxel& v, size_t x, size_t z)
{
    if (x > 0 && x < CHUNK_WIDTH - 1)
    {
        if (z == CHUNK_WIDTH - 1)
            AddEdgeMesh(meshBuilder, v, FRONT);
        else if (z == 0)
            AddEdgeMesh(meshBuilder, v, BACK);
    }
    if (z > 0 && z < CHUNK_WIDTH - 1)
    {
        if (x == CHUNK_WIDTH - 1)
            AddEdgeMesh(meshBuilder, v, RIGHT);
        else if (x == 0)
            AddEdgeMesh(meshBuilder, v, LEFT);
    }
    if (x == 0 && z == 0)
        AddEdgeMesh(meshBuilder, v, BACK, LEFT);
    if (x == 0 && z == CHUNK_WIDTH - 1)
        AddEdgeMesh(meshBuilder, v, FRONT, LEFT);
    if (x == CHUNK_WIDTH - 1 && z == 0)
        AddEdgeMesh(meshBuilder, v, BACK, RIGHT);
    if (x == CHUNK_WIDTH - 1 && z == CHUNK_WIDTH - 1)
        AddEdgeMesh(meshBuilder, v, FRONT, RIGHT);
}

void Chunk::AddEdgeMesh(VoxelMeshBuilder& meshBuilder, Voxel& v, VoxelFace f)
{
    bool faces[6] = {false, false, false, false, false, false};
    faces[f] = true;
    std::vector<VoxelVertex> data = meshBuilder.FromVoxelFaces(v, faces);
    m_BorderMeshes.at(f).insert(m_BorderMeshes.at(f).begin(), data.begin(), data.end());
    data = meshBuilder.FromVoxelExceptFaces(v, faces);
    m_Mesh.insert(m_Mesh.begin(), data.begin(), data.end());
}

void Chunk::AddEdgeMesh(VoxelMeshBuilder& meshBuilder, Voxel& v, VoxelFace f1, VoxelFace f2)
{
    std::vector<VoxelVertex> data;
    bool faces[6] = {false, false, false, false, false, false};
    faces[f1] = true;
    data = meshBuilder.FromVoxelFaces(v, faces);
    m_BorderMeshes.at(f1).insert(m_BorderMeshes.at(f1).begin(), data.begin(), data.end());
    faces[f1] = false;
    faces[f2] = true;
    data = meshBuilder.FromVoxelFaces(v, faces);
    m_BorderMeshes.at(f2).insert(m_BorderMeshes.at(f2).begin(), data.begin(), data.end());
    faces[f1] = true;
    data = meshBuilder.FromVoxelExceptFaces(v, faces);
    m_Mesh.insert(m_Mesh.begin(), data.begin(), data.end());
}

void Chunk::DetermineVoxelFeatures(Voxel& v, size_t x, size_t z, size_t h)
{
    int32_t y = &v - &m_VoxelGrid[x][z][0];
    if (y >= h)
    {
        v.SetPosition(Position3D(x, y, z));
        return;
    }
    const int32_t snowThreshold = 3 * CHUNK_HEIGHT / 5;
    double_t density = m_Perlin.octave3D((static_cast<double_t>(m_Position.x) * CHUNK_WIDTH + x) * 0.02,
                                         (static_cast<double_t>(m_Position.y) * CHUNK_WIDTH + z) * 0.02,
                                         y * 0.02,
                                         4);
    VoxelType type = AIR;
    density += 1 - static_cast<double_t>(y + h / 4) / CHUNK_HEIGHT;
    if (density >= 0)
    {
        type = STONE;
        if (y > snowThreshold)
        {
            if (y == snowThreshold + 1)
                type = DIRT_SNOW;
            else
                type = SNOW;
        } else
        {
            if (y > h - 5)
                type = DIRT;
            if (y == h - 1)
                type = GRASS;
        }
    }
    if (y == 0)
        type = STONE;
    v.SetPosition(Position3D(x, y, z));
    v.SetVoxelType(type);
    ++y;
}

VoxelGrid& Chunk::GetVoxelGrid()
{
    return m_VoxelGrid;
}

const std::vector<VoxelVertex>& Chunk::GetMesh() const
{
    return m_Mesh;
}

const std::vector<VoxelVertex>& Chunk::GetBorderMesh(const VoxelFace face) const
{
    return m_BorderMeshes.at(face);
}

Position2D Chunk::GetPosition() const
{
    return m_Position;
}

std::mutex& Chunk::GetLock()
{
    return m_Mutex;
}

glm::mat4 Chunk::GetModelMatrix() const
{
    constexpr glm::mat4 model(1.0f);
    auto pos = glm::vec3(m_Position.x, 0, m_Position.y);
    pos.x *= CHUNK_WIDTH;
    pos.y *= CHUNK_HEIGHT;
    pos.z *= CHUNK_WIDTH;
    return glm::translate(model, pos);
}

};
