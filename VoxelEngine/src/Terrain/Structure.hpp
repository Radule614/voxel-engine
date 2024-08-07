#pragma once

#include "Voxel.hpp"
#include <vector>

namespace VoxelEngine
{

class Structure
{
public:
	Voxel& GetRoot();
	int32_t GetRadius() const;
	const std::vector<std::pair<glm::i16vec3, VoxelType>>& GetVoxelData();
	void SetRootPosition(Position3D p);

protected:
	Structure(VoxelType rootType);

protected:
	Voxel m_Root;
	int32_t m_Radius;
	std::vector<std::pair<glm::i16vec3, VoxelType>> m_VoxelData;
};

}