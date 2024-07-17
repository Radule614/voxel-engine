#include "Structure.hpp"
#include "../Macros.hpp"

namespace VoxelEngine
{

Structure::Structure(VoxelType rootType) : m_Radius(0), m_Voxels({}), m_Root(rootType) {}

Voxel& Structure::GetRoot()
{
	return m_Root;
}

int32_t Structure::GetRadius() const
{
	return m_Radius;
}

const std::vector<Voxel>& Structure::GetVoxels()
{
	return m_Voxels;
}

void Structure::SetRootPosition(Position3D p)
{
	assert(p.x >= 0 && p.x <= CHUNK_WIDTH - 1 && p.y >= 0 && p.y <= CHUNK_HEIGHT - 1 && p.z >= 0 && p.z <= CHUNK_WIDTH - 1);
	m_Root.SetPosition(p);
}

}