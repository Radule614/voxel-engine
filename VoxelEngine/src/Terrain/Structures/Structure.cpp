#include "Structure.hpp"

namespace VoxelEngine
{

Structure::Structure(const VoxelType rootType, const std::vector<std::pair<glm::i32vec3, VoxelType> >& data)
    : m_Root(rootType), m_Radius(0), m_Data(data)
{
}

const std::vector<std::pair<glm::i32vec3, VoxelType> >& Structure::GetVoxelData() const { return m_Data; }

Structure::~Structure() = default;

void Structure::SetRootPosition(const Position3D position) { m_Root.SetPosition(position); }

void Structure::SetRadius(const int32_t radius) { m_Radius = radius; }

const Voxel& Structure::GetRoot() const { return m_Root; }

int32_t Structure::GetRadius() const { return m_Radius; }

}
