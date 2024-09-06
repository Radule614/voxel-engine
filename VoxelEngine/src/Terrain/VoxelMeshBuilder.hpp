#pragma once

#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>

#include "Voxel.hpp"
#include "../Assets/Vertex.hpp"

namespace VoxelEngine
{

class VoxelMeshBuilder
{
public:
	VoxelMeshBuilder();
	~VoxelMeshBuilder();

	std::vector<Vertex> FromVoxel(Voxel& voxel);
	std::vector<Vertex> FromVoxel(Voxel& voxel, VoxelFace f);
	std::vector<Vertex> FromVoxelFaces(Voxel& voxel, bool faces[6]);
	std::vector<Vertex> FromVoxelExceptFaces(Voxel& voxel, bool faces[6]);

private:
	static void Init();

private:
	static bool s_Initialized;
	static std::vector<glm::vec2> s_TextureCoordinates;
	static std::unordered_map<VoxelFace, std::vector<glm::vec3>> s_PositionMap;
	static std::unordered_map<VoxelFace, glm::vec3> s_NormalMap;
	static std::unordered_map<VoxelType, std::vector<int32_t>> s_FaceTextureMap;
};

};
