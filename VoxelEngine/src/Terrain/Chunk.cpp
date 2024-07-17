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
	m_BorderMeshes.insert({ VoxelFace::FRONT, {} });
	m_BorderMeshes.insert({ VoxelFace::RIGHT, {} });
	m_BorderMeshes.insert({ VoxelFace::BACK, {} });
	m_BorderMeshes.insert({ VoxelFace::LEFT, {} });
}

Chunk::~Chunk()
{
}

void Chunk::Generate()
{
	size_t heightMap[CHUNK_WIDTH][CHUNK_WIDTH]{};

	for (size_t x = 0; x < CHUNK_WIDTH; ++x)
	{
		for (size_t z = 0; z < CHUNK_WIDTH; ++z)
		{
			const double_t height_bias = m_Perlin.octave2D_01(((double_t)m_Position.x * CHUNK_WIDTH + x) * 0.02,
				((double_t)m_Position.y * CHUNK_WIDTH + z) * 0.02,
				4);
			size_t h = 2.0 * CHUNK_HEIGHT / 3 + glm::floor(height_bias * CHUNK_HEIGHT / 3) - 16;
			heightMap[x][z] = h;
			std::for_each(std::execution::par,
				std::begin(m_VoxelGrid[x][z]),
				std::end(m_VoxelGrid[x][z]),
				[&](Voxel& v) { this->DetermineVoxelFeatures(v, x, z, h); });
		}
	}

	Position3D p(CHUNK_WIDTH / 2, heightMap[CHUNK_WIDTH / 2][CHUNK_WIDTH / 2] + 1, CHUNK_WIDTH / 2);
	Tree tree(p);
	m_VoxelGrid[p.x][p.z][p.y].SetVoxelType(tree.GetRoot().GetVoxelType());
	for (auto& v : tree.GetVoxels())
	{
		auto pair = GetPositionRelativeToWorld(tree.GetRoot().GetPosition() + v.GetPosition());
		if (pair.first == m_Position)
		{
			Position3D& t = pair.second;
			LOG_INFO(t.ToString());
			m_VoxelGrid[t.x][t.z][t.y].SetVoxelType(v.GetVoxelType());
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

				if (x == 0 || z == 0 || x == CHUNK_WIDTH - 1 || z == CHUNK_WIDTH - 1)
					DetermineEdgeMeshes(meshBuilder, v, x, z);
				else
				{
					std::vector<Vertex> data = meshBuilder.FromVoxel(v);
					m_Mesh.insert(m_Mesh.begin(), data.begin(), data.end());
				}
			}
		}
	}
}

void Chunk::GenerateEdgeMesh(VoxelFace face)
{
	std::vector<Vertex>& mesh = m_BorderMeshes.at(face);
	mesh.clear();
	VoxelMeshBuilder meshBuilder;
	for (size_t x = 0; x < CHUNK_WIDTH; x++)
	{
		for (size_t y = 0; y < CHUNK_HEIGHT; y++)
		{
			Voxel v = m_VoxelGrid[x][x][y];
			if (face == VoxelFace::FRONT)
				v = m_VoxelGrid[x][CHUNK_WIDTH - 1][y];
			else if (face == VoxelFace::BACK)
				v = m_VoxelGrid[x][0][y];
			else if (face == VoxelFace::RIGHT)
				v = m_VoxelGrid[CHUNK_WIDTH - 1][x][y];
			else if (face == VoxelFace::LEFT)
				v = m_VoxelGrid[0][x][y];
			if (!v.IsFaceVisible(face))
				continue;
			std::vector<Vertex> data = meshBuilder.FromVoxel(v, face);
			mesh.insert(mesh.end(), data.begin(), data.end());
		}
	}
}

std::pair<Position2D, Position3D> Chunk::GetPositionRelativeToWorld(Position3D pos) const
{
	if (InRange(pos.x, 0, CHUNK_WIDTH - 1) && InRange(pos.y, 0, CHUNK_WIDTH - 1) && InRange(pos.z, 0, CHUNK_WIDTH - 1))
		return { m_Position, pos };
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
	return { chunkPos, pos };
}

void Chunk::DetermineEdgeMeshes(VoxelMeshBuilder& meshBuilder, Voxel& v, size_t x, size_t z)
{
	if (x > 0 && x < CHUNK_WIDTH - 1)
	{
		if (z == CHUNK_WIDTH - 1)
			AddEdgeMesh(meshBuilder, v, VoxelFace::FRONT);
		else if (z == 0)
			AddEdgeMesh(meshBuilder, v, VoxelFace::BACK);
	}
	if (z > 0 && z < CHUNK_WIDTH - 1)
	{
		if (x == CHUNK_WIDTH - 1)
			AddEdgeMesh(meshBuilder, v, VoxelFace::RIGHT);
		else if (x == 0)
			AddEdgeMesh(meshBuilder, v, VoxelFace::LEFT);
	}
	if (x == 0 && z == 0)
		AddEdgeMesh(meshBuilder, v, VoxelFace::BACK, VoxelFace::LEFT);
	if (x == 0 && z == CHUNK_WIDTH - 1)
		AddEdgeMesh(meshBuilder, v, VoxelFace::FRONT, VoxelFace::LEFT);
	if (x == CHUNK_WIDTH - 1 && z == 0)
		AddEdgeMesh(meshBuilder, v, VoxelFace::BACK, VoxelFace::RIGHT);
	if (x == CHUNK_WIDTH - 1 && z == CHUNK_WIDTH - 1)
		AddEdgeMesh(meshBuilder, v, VoxelFace::FRONT, VoxelFace::RIGHT);
}

void Chunk::AddEdgeMesh(VoxelMeshBuilder& meshBuilder, Voxel& v, VoxelFace f)
{
	bool faces[6] = { false, false, false, false, false, false };
	faces[f] = true;
	std::vector<Vertex> data = meshBuilder.FromVoxelFaces(v, faces);
	m_BorderMeshes.at(f).insert(m_BorderMeshes.at(f).begin(), data.begin(), data.end());
	data = meshBuilder.FromVoxelExceptFaces(v, faces);
	m_Mesh.insert(m_Mesh.begin(), data.begin(), data.end());
}

void Chunk::AddEdgeMesh(VoxelMeshBuilder& meshBuilder, Voxel& v, VoxelFace f1, VoxelFace f2)
{
	std::vector<Vertex> data;
	bool faces[6] = { false, false, false, false, false, false };
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
	size_t y = &v - &m_VoxelGrid[x][z][0];
	if (y >= h)
		return;
	double_t density = m_Perlin.octave3D(((double_t)m_Position.x * CHUNK_WIDTH + x) * 0.02,
		((double_t)m_Position.y * CHUNK_WIDTH + z) * 0.02,
		y * 0.02,
		5);
	VoxelType type = VoxelType::AIR;
	density += (1.0 / CHUNK_HEIGHT) * (CHUNK_HEIGHT - h / 1.1);
	if (density >= 0 || y == 0)
		type = VoxelType::STONE;

	if (density >= 0 && y > h - 3)
		type = VoxelType::DIRT;
	if (density >= 0 && y == h - 1)
		type = VoxelType::GRASS;

	v.SetPosition(Position3D(x, y, z));
	v.SetVoxelType(type);
	++y;
}

VoxelGrid& Chunk::GetVoxelGrid()
{
	return m_VoxelGrid;
}

const std::vector<Vertex>& Chunk::GetMesh() const
{
	return m_Mesh;
}

const std::vector<Vertex>& Chunk::GetBorderMesh(VoxelFace face) const
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
	glm::mat4 model(1.0f);
	glm::vec3 pos = glm::vec3(m_Position.x, 0, m_Position.y);
	pos.x *= CHUNK_WIDTH;
	pos.y *= CHUNK_HEIGHT;
	pos.z *= CHUNK_WIDTH;
	return glm::translate(model, pos);
}

};
