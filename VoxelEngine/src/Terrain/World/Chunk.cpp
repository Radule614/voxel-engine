#include "Chunk.hpp"
#include "GLCoreUtils.hpp"
#include "../Voxel/VoxelMeshBuilder.hpp"
#include "../Utils/Position2D.hpp"
#include "../../Utils/Utils.hpp"
#include "World.hpp"
#include "../../Ecs/Ecs.hpp"

#include <execution>

#include "../../Ecs/Components/LightComponent.hpp"

namespace VoxelEngine
{

Chunk::Chunk(World& world, const Biome& biome) : Chunk(world, Position2D(), biome)
{
}

Chunk::Chunk(World& world, const Position2D position, const Biome& biome)
    : m_World(world),
      m_Position(position),
      m_VoxelGrid(),
      m_Lock(std::mutex()),
      m_Biome(biome),
      m_BiomeLock(std::mutex())
{
    m_BorderMeshes.insert({FRONT, {}});
    m_BorderMeshes.insert({RIGHT, {}});
    m_BorderMeshes.insert({BACK, {}});
    m_BorderMeshes.insert({LEFT, {}});
}

Chunk::~Chunk()
{
    if (EntityComponentSystem::HasShutdown())
        return;

    auto& registry = EntityComponentSystem::Instance().GetEntityRegistry();

    for (const auto entityId: m_Entities)
    {
        auto& light = registry.get<LightComponent>(entityId).PointLight;

        if (light.DepthCubeMap != 0)
        {
            glDeleteTextures(1, &light.DepthCubeMap);
            light.DepthCubeMap = 0;
        }

        registry.destroy(entityId);
    }

    m_Entities.clear();
}

void Chunk::Generate()
{
    int32_t heightMap[CHUNK_WIDTH][CHUNK_WIDTH]{};
    for (size_t x = 0; x < CHUNK_WIDTH; ++x)
    {
        for (size_t z = 0; z < CHUNK_WIDTH; ++z)
        {
            const glm::ivec3 globalPosition = World::WorldToGlobalSpace(m_Position, Position3D(x, 0, z));
            int32_t h = m_Biome.GetHeight(globalPosition.x, globalPosition.z);

            heightMap[x][z] = h;
            std::for_each(std::execution::par,
                          std::begin(m_VoxelGrid[x][z]),
                          std::end(m_VoxelGrid[x][z]),
                          [&](Voxel& v) { this->DetermineVoxelFeatures(v, x, z, h); });
        }
    }

    Voxel surfaceLayer[CHUNK_WIDTH][CHUNK_WIDTH] = {};
    for (size_t x = 0; x < CHUNK_WIDTH; ++x)
    {
        for (size_t z = 0; z < CHUNK_WIDTH; ++z)
        {
            const auto surfaceY = heightMap[x][z] - 1;
            if (surfaceY < 0)
                continue;

            surfaceLayer[x][z] = m_VoxelGrid[x][z][surfaceY];
        }
    }

    const Biome::GeneratorContext context(*this, surfaceLayer, m_BiomeTypes);
    std::vector<Structure> output{};
    m_Biome.GenerateStructures(context, output);
    AddStructures(output);
}

void Chunk::AddStructures(const std::vector<Structure>& structures)
{
    std::unordered_set<Chunk*> changedChunks{};
    auto& deferredQueueMap = m_World.GetDeferredUpdateQueueMap();

    for (auto& structure: structures)
    {
        Position3D rootPosition = structure.GetRoot().GetPosition();
        Voxel& rootVoxel = GetVoxelFromGrid(rootPosition);

        rootVoxel.SetVoxelType(structure.GetRoot().GetVoxelType());
        rootVoxel.SetPosition(rootPosition);

        for (auto& [positionInStructure, voxelType]: structure.GetVoxelData())
        {
            glm::i32vec3 position = static_cast<glm::i32vec3>(rootPosition) + positionInStructure;
            auto [chunkPosition, voxelPosition] = GetPositionRelativeToWorld(position);

            if (chunkPosition == m_Position)
            {
                Voxel& voxel = GetVoxelFromGrid(voxelPosition);

                voxel.SetVoxelType(voxelType);
                voxel.SetPosition(voxelPosition);

                continue;
            }

            if (m_World.GetChunkMap().contains(chunkPosition))
            {
                auto& chunk = m_World.GetChunkMap().at(chunkPosition);

                chunk->GetLock().lock();

                auto& voxelGrid = chunk->GetVoxelGrid();
                Voxel& voxel = voxelGrid[voxelPosition.GetX()][voxelPosition.GetZ()][voxelPosition.GetY()];
                voxel.SetVoxelType(voxelType);
                voxel.SetPosition(voxelPosition);
                changedChunks.insert(chunk.get());

                chunk->GetLock().unlock();

                continue;
            }
            deferredQueueMap[chunkPosition].emplace(voxelType, voxelPosition);
        }
    }

    m_World.GetLock().lock();
    for (auto& c: changedChunks)
    {
        c->GetLock().lock();
        c->GenerateMesh();
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

// TODO: Refactor this method
std::pair<Position2D, Position3D> Chunk::GetPositionRelativeToWorld(glm::i32vec3 pos) const
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
    bool faces[6] = {false, false, false, false, false, false};
    faces[f1] = true;
    std::vector<VoxelVertex> data = meshBuilder.FromVoxelFaces(v, faces);
    m_BorderMeshes.at(f1).insert(m_BorderMeshes.at(f1).begin(), data.begin(), data.end());
    faces[f1] = false;
    faces[f2] = true;
    data = meshBuilder.FromVoxelFaces(v, faces);
    m_BorderMeshes.at(f2).insert(m_BorderMeshes.at(f2).begin(), data.begin(), data.end());
    faces[f1] = true;
    data = meshBuilder.FromVoxelExceptFaces(v, faces);
    m_Mesh.insert(m_Mesh.begin(), data.begin(), data.end());
}

void Chunk::DetermineVoxelFeatures(Voxel& v, size_t x, size_t z, int32_t h)
{
    const int32_t y = &v - &m_VoxelGrid[x][z][0];
    v.SetPosition(Position3D(x, y, z));

    if (y >= h)
        return;

    const auto globalPosition = World::WorldToGlobalSpace(m_Position, v.GetPosition());

    const auto [biomeType, voxelType] = m_Biome.ResolveBiomeFeatures(globalPosition, h);

    v.SetVoxelType(voxelType);

    if (!m_BiomeTypes.contains(biomeType))
    {
        m_BiomeLock.lock();

        m_BiomeTypes.insert(biomeType);

        m_BiomeLock.unlock();
    }
}

VoxelGrid& Chunk::GetVoxelGrid() { return m_VoxelGrid; }

Voxel& Chunk::GetVoxelFromGrid(const Position3D positionInGrid)
{
    return m_VoxelGrid[positionInGrid.GetX()][positionInGrid.GetZ()][positionInGrid.GetY()];
}

void Chunk::AddPointLight(const glm::vec3 position, const glm::vec3 color)
{
    auto& registry = EntityComponentSystem::Instance().GetEntityRegistry();
    const auto entityId = registry.create();

    const glm::vec3 globalPosition = (glm::vec3)GetWorldPosition() + position;

    PointLight pointLight(globalPosition, color);
    registry.emplace<LightComponent>(entityId, std::move(pointLight));
    m_Entities.push_back(entityId);
}

const std::vector<VoxelVertex>& Chunk::GetMesh() const { return m_Mesh; }

const std::vector<VoxelVertex>& Chunk::GetBorderMesh(const VoxelFace face) const { return m_BorderMeshes.at(face); }

Position2D Chunk::GetPosition() const { return m_Position; }

glm::ivec3 Chunk::GetWorldPosition() const { return {m_Position.x * CHUNK_WIDTH, 0, m_Position.y * CHUNK_WIDTH}; }

std::mutex& Chunk::GetLock() { return m_Lock; }

glm::mat4 Chunk::GetModelMatrix() const
{
    auto pos = glm::vec3(m_Position.x, 0, m_Position.y);
    pos.x *= CHUNK_WIDTH;
    pos.y *= CHUNK_HEIGHT;
    pos.z *= CHUNK_WIDTH;
    return glm::translate(glm::mat4(1.0f), pos);
}

}
