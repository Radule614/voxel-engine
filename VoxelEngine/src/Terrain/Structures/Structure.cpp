#include "Structure.hpp"

namespace VoxelEngine
{

Structure::Structure(const VoxelType rootType, const std::vector<std::pair<glm::i32vec3, VoxelType> >& data)
    : m_Root(rootType), m_Data(data)
{
}

Structure::~Structure() = default;

const std::vector<std::pair<glm::i32vec3, VoxelType> >& Structure::GetVoxelData() const { return m_Data; }

void Structure::SetRootPosition(const Position3D position) { m_Root.SetPosition(position); }

const Voxel& Structure::GetRoot() const { return m_Root; }

}
