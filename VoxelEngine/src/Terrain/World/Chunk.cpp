#include "Chunk.hpp"

#include <execution>

#include "GLCoreUtils.hpp"
#include "../Voxel/VoxelMeshBuilder.hpp"
#include "../Utils/Position2D.hpp"
#include "../../Utils/Utils.hpp"
#include "World.hpp"
#include "../../Ecs/Ecs.hpp"
#include "../../Assets/AssetManager.hpp"
#include "../../Ecs/Components/LightComponent.hpp"
#include "../../Ecs/Components/TerrainMeshComponent.hpp"
#include "../../Ecs/Components/TransformComponent.hpp"

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

    auto& registry = EntityComponentSystem::Instance().GetEntityRegistry();
    m_EntityId = registry.create();

    auto worldPosition = glm::vec3(0.0f);
    worldPosition.x = m_Position.x * CHUNK_WIDTH;
    worldPosition.z = m_Position.y * CHUNK_WIDTH;

    TransformComponent transform{};
    transform.Position = worldPosition;

    registry.emplace<TransformComponent>(m_EntityId, transform);
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

    if (registry.all_of<TerrainMeshComponent>(m_EntityId))
    {
        const auto& terrainMesh = registry.get<TerrainMeshComponent>(m_EntityId);

        glBindVertexArray(0);
        glDeleteBuffers(1, &terrainMesh.VertexBuffer);
        glDeleteBuffers(1, &terrainMesh.IndexBuffer);
        glDeleteVertexArrays(1, &terrainMesh.VertexArray);

        registry.remove<TerrainMeshComponent>(m_EntityId);
    }

    if (registry.valid(m_EntityId))
        registry.destroy(m_EntityId);
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

                std::lock_guard lock(chunk->GetLock());

                auto& voxelGrid = chunk->GetVoxelGrid();
                Voxel& voxel = voxelGrid[voxelPosition.GetX()][voxelPosition.GetZ()][voxelPosition.GetY()];
                voxel.SetVoxelType(voxelType);
                voxel.SetPosition(voxelPosition);
                changedChunks.insert(chunk.get());

                continue;
            }
            deferredQueueMap[chunkPosition].emplace(voxelType, voxelPosition);
        }
    }

    {
        std::lock_guard lock(m_World.GetLock());

        for (auto& c: changedChunks)
        {
            c->GetLock().lock();
            c->GenerateMesh();
            c->GetLock().unlock();
        }
    }
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
std::pair<Position2D, Position3D> Chunk::GetPositionRelativeToWorld(glm::i32vec3 position) const
{
    if (InRange(position.x, 0, CHUNK_WIDTH - 1) &&
        InRange(position.y, 0, CHUNK_WIDTH - 1) &&
        InRange(position.z, 0,CHUNK_WIDTH - 1)) { return {m_Position, Position3D(position.x, position.y, position.z)}; }

    Position2D chunkPosition = m_Position;
    while (position.x < 0)
    {
        position.x += CHUNK_WIDTH;
        --chunkPosition.x;
    }
    while (position.x > CHUNK_WIDTH - 1)
    {
        position.x -= CHUNK_WIDTH;
        ++chunkPosition.x;
    }
    while (position.z < 0)
    {
        position.z += CHUNK_WIDTH;
        --chunkPosition.y;
    }
    while (position.z > CHUNK_WIDTH - 1)
    {
        position.z -= CHUNK_WIDTH;
        ++chunkPosition.y;
    }
    return {chunkPosition, Position3D(position.x, position.y, position.z)};
}

void Chunk::DetermineEdgeMeshes(VoxelMeshBuilder& meshBuilder, Voxel& voxel, size_t x, size_t z)
{
    if (x > 0 && x < CHUNK_WIDTH - 1)
    {
        if (z == CHUNK_WIDTH - 1)
            AddEdgeMesh(meshBuilder, voxel, FRONT);
        else if (z == 0)
            AddEdgeMesh(meshBuilder, voxel, BACK);
    }
    if (z > 0 && z < CHUNK_WIDTH - 1)
    {
        if (x == CHUNK_WIDTH - 1)
            AddEdgeMesh(meshBuilder, voxel, RIGHT);
        else if (x == 0)
            AddEdgeMesh(meshBuilder, voxel, LEFT);
    }
    if (x == 0 && z == 0)
        AddEdgeMesh(meshBuilder, voxel, BACK, LEFT);
    if (x == 0 && z == CHUNK_WIDTH - 1)
        AddEdgeMesh(meshBuilder, voxel, FRONT, LEFT);
    if (x == CHUNK_WIDTH - 1 && z == 0)
        AddEdgeMesh(meshBuilder, voxel, BACK, RIGHT);
    if (x == CHUNK_WIDTH - 1 && z == CHUNK_WIDTH - 1)
        AddEdgeMesh(meshBuilder, voxel, FRONT, RIGHT);
}

void Chunk::AddEdgeMesh(VoxelMeshBuilder& meshBuilder, Voxel& voxel, const VoxelFace face)
{
    bool faces[6] = {false, false, false, false, false, false};
    faces[face] = true;

    std::vector<VoxelVertex> data = meshBuilder.FromVoxelFaces(voxel, faces);
    m_BorderMeshes.at(face).insert(m_BorderMeshes.at(face).begin(), data.begin(), data.end());

    data = meshBuilder.FromVoxelExceptFaces(voxel, faces);
    m_Mesh.insert(m_Mesh.begin(), data.begin(), data.end());
}

void Chunk::AddEdgeMesh(VoxelMeshBuilder& meshBuilder, Voxel& voxel, const VoxelFace face1, const VoxelFace face2)
{
    bool faces[6] = {false, false, false, false, false, false};
    faces[face1] = true;

    std::vector<VoxelVertex> data = meshBuilder.FromVoxelFaces(voxel, faces);
    m_BorderMeshes.at(face1).insert(m_BorderMeshes.at(face1).begin(), data.begin(), data.end());

    faces[face1] = false;
    faces[face2] = true;
    data = meshBuilder.FromVoxelFaces(voxel, faces);
    m_BorderMeshes.at(face2).insert(m_BorderMeshes.at(face2).begin(), data.begin(), data.end());

    faces[face1] = true;
    data = meshBuilder.FromVoxelExceptFaces(voxel, faces);
    m_Mesh.insert(m_Mesh.begin(), data.begin(), data.end());
}

void Chunk::DetermineVoxelFeatures(Voxel& voxel, size_t x, size_t z, int32_t h)
{
    const int32_t y = &voxel - &m_VoxelGrid[x][z][0];
    voxel.SetPosition(Position3D(x, y, z));

    if (y >= h)
        return;

    const auto globalPosition = World::WorldToGlobalSpace(m_Position, voxel.GetPosition());

    const auto [biomeType, voxelType] = m_Biome.ResolveBiomeFeatures(globalPosition, h);

    voxel.SetVoxelType(voxelType);

    if (!m_BiomeTypes.contains(biomeType))
    {
        std::lock_guard lock(m_BiomeLock);

        m_BiomeTypes.insert(biomeType);
    }
}

void Chunk::CreateTerrainMeshComponent() const
{
    auto& registry = EntityComponentSystem::Instance().GetEntityRegistry();

    const Texture albedoTexture = AssetManager::Instance().LoadTexture("assets/textures/atlas.png", "Diffuse");

    Material terrainMaterial{};
    terrainMaterial.AlbedoFactor = glm::vec4(1.0f);
    terrainMaterial.AlbedoTextureId = albedoTexture.id;
    terrainMaterial.MetallicFactor = 0.0f;
    terrainMaterial.RoughnessFactor = 0.85f;

    TerrainMeshComponent terrainMeshComponent(m_Position, terrainMaterial);

    glCreateVertexArrays(1, &terrainMeshComponent.VertexArray);
    glBindVertexArray(terrainMeshComponent.VertexArray);

    glCreateBuffers(1, &terrainMeshComponent.VertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, terrainMeshComponent.VertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VoxelVertex), nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(VoxelVertex),
                          reinterpret_cast<void*>(offsetof(VoxelVertex, Normal)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(VoxelVertex),
                          reinterpret_cast<void*>(offsetof(VoxelVertex, TexCoords)));

    glCreateBuffers(1, &terrainMeshComponent.IndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainMeshComponent.IndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, nullptr,GL_DYNAMIC_DRAW);

    glBindVertexArray(0);

    registry.emplace<TerrainMeshComponent>(m_EntityId, terrainMeshComponent);
}

void Chunk::UpdateTerrainMeshComponent() const
{
    auto& registry = EntityComponentSystem::Instance().GetEntityRegistry();

    if (!registry.all_of<TerrainMeshComponent>(m_EntityId))
        CreateTerrainMeshComponent();

    TerrainMeshComponent& terrainMeshComponent = registry.get<TerrainMeshComponent>(m_EntityId);

    std::vector<VoxelVertex> vertices = {};
    vertices.insert(vertices.end(), GetMesh().begin(), GetMesh().end());
    vertices.insert(vertices.end(),
                    GetBorderMesh(FRONT).begin(),
                    GetBorderMesh(FRONT).end());
    vertices.insert(vertices.end(),
                    GetBorderMesh(RIGHT).begin(),
                    GetBorderMesh(RIGHT).end());
    vertices.insert(vertices.end(),
                    GetBorderMesh(BACK).begin(),
                    GetBorderMesh(BACK).end());
    vertices.insert(vertices.end(),
                    GetBorderMesh(LEFT).begin(),
                    GetBorderMesh(LEFT).end());

    if (vertices.empty())
        return;

    glBindVertexArray(terrainMeshComponent.VertexArray);

    glBindBuffer(GL_ARRAY_BUFFER, terrainMeshComponent.VertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VoxelVertex), &vertices[0], GL_DYNAMIC_DRAW);

    terrainMeshComponent.Indices.clear();
    terrainMeshComponent.Indices.reserve(vertices.size() / 4 * 6);

    const uint32_t faceCount = vertices.size() / 4;
    for (uint32_t i = 0; i < faceCount; ++i)
    {
        terrainMeshComponent.Indices.emplace_back(i * 4 + 0);
        terrainMeshComponent.Indices.emplace_back(i * 4 + 1);
        terrainMeshComponent.Indices.emplace_back(i * 4 + 2);
        terrainMeshComponent.Indices.emplace_back(i * 4 + 2);
        terrainMeshComponent.Indices.emplace_back(i * 4 + 3);
        terrainMeshComponent.Indices.emplace_back(i * 4 + 0);
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainMeshComponent.IndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 terrainMeshComponent.Indices.size() * sizeof(uint32_t),
                 &terrainMeshComponent.Indices[0],
                 GL_DYNAMIC_DRAW);

    glBindVertexArray(0);
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

    const glm::vec3 globalPosition = (glm::vec3) GetWorldPosition() + position;

    PointLight pointLight(globalPosition, color);
    registry.emplace<LightComponent>(entityId, std::move(pointLight));
    m_Entities.push_back(entityId);
}

const std::vector<VoxelVertex>& Chunk::GetMesh() const { return m_Mesh; }

const std::vector<VoxelVertex>& Chunk::GetBorderMesh(const VoxelFace face) const { return m_BorderMeshes.at(face); }

Position2D Chunk::GetPosition() const { return m_Position; }

glm::ivec3 Chunk::GetWorldPosition() const { return {m_Position.x * CHUNK_WIDTH, 0, m_Position.y * CHUNK_WIDTH}; }

std::mutex& Chunk::GetLock() { return m_Lock; }

}
