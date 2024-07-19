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
	const std::vector<Voxel>& GetVoxels();
	void SetRootPosition(Position3D p);

protected:
	Structure(VoxelType rootType);

protected:
	Voxel m_Root;
	int32_t m_Radius;
	std::vector<Voxel> m_Voxels;
};

}