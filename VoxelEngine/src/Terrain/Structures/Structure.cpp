#include "Structure.hpp"

namespace VoxelEngine
{

Structure::Structure(const VoxelType rootType) : m_Root(rootType), m_Radius(0), m_VoxelData({})
{
}

const Voxel& Structure::GetRoot() const { return m_Root; }

int32_t Structure::GetRadius() const { return m_Radius; }

const std::vector<std::pair<glm::i32vec3, VoxelType> >& Structure::GetVoxelData() const { return m_VoxelData; }

void Structure::SetRootPosition(Position3D position) { m_Root.SetPosition(position); }

}
