#pragma once

#include "Structure.hpp"

namespace VoxelEngine
{

class Tree : public Structure
{
public:
	Tree(Position3D p) : Structure(VoxelType::GRASS)
	{
		m_Root.SetPosition(p);
		m_Radius = 3;
		for (size_t i = 0; i < 16; ++i)
			m_Voxels.push_back(Voxel(VoxelType::GRASS, Position3D(0, i + 1, 0)));
	}
};

}