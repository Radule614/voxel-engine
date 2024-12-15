#pragma once

#include <glm/glm.hpp>

namespace VoxelEngine
{

struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec3 Tangent;
};

struct VoxelVertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	uint8_t Light;
};

};
