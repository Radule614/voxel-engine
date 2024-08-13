#pragma once
#include "Vertex.hpp"
#include "Texture.hpp"

namespace VoxelEngine
{

class Mesh
{
public:

	Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices, std::vector<Texture> textures);

private:
	void SetupMesh();

private:
	uint32_t m_VAO, m_VBO, m_EBO;
	std::vector<Vertex> m_Vertices;
	std::vector<uint32_t> m_Indices;
	std::vector<Texture> m_Textures;
};

}