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
		for (size_t y = 1; y <= 7; ++y)
			m_Voxels.push_back(Voxel(VoxelType::GRASS, Position3D(0, y, 0)));
		for (int32_t x = -2; x <= 2; ++x)
		{
			for (int32_t z = -2; z <= 2; ++z)
			{
				for (size_t y = 6; y <= 9; ++y)
				{
					if (x == 0 && z == 0 && y <= 7)
						continue;
					m_Voxels.push_back(Voxel(VoxelType::GRASS, Position3D(x, y, z)));
				}
			}
		}
	}
};

}