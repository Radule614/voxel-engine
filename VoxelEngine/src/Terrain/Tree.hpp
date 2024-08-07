#pragma once

#include "Structure.hpp"

namespace VoxelEngine
{

class Tree : public Structure
{
public:
	Tree(Position3D p) : Structure(VoxelType::LOG)
	{
		m_Root.SetPosition(p);
		m_Radius = 4;
		for (size_t y = 1; y <= 7; ++y)
			m_VoxelData.push_back({ glm::i16vec3(0, y, 0), VoxelType::LOG });
		for (int32_t x = -3; x <= 3; ++x)
		{
			for (int32_t z = -3; z <= 3; ++z)
			{

				for (size_t y = 5; y <= 9; ++y)
				{
					if (x == 0 && z == 0 && y <= 7)
						continue;
					if ((x == -3 || x == 3 || z == -3 || z == 3) && (y == 5 || y == 9))
						continue;
					m_VoxelData.push_back({ glm::i16vec3(x, y, z), VoxelType::LEAVES });
				}
			}
		}
	}
};

class LargeTree : public Structure
{
public:
	LargeTree(Position3D p) : Structure(VoxelType::LOG)
	{
		m_Root.SetPosition(p);
		m_Radius = 5;
		for (size_t y = 1; y <= 10; ++y)
			m_VoxelData.push_back({ glm::i16vec3(0, y, 0), VoxelType::LOG });
		for (int32_t x = -3; x <= 3; ++x)
		{
			for (int32_t z = -3; z <= 3; ++z)
			{

				for (size_t y = 8; y <= 13; ++y)
				{
					if (x == 0 && z == 0 && y <= 10)
						continue;
					if ((x == -3 || x == 3 || z == -3 || z == 3) && (y == 8 || y == 13))
						continue;
					m_VoxelData.push_back({ glm::i16vec3(x, y, z), VoxelType::LEAVES });
				}
			}
		}
	}
};

}