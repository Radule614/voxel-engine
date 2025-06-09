#include "Structure.hpp"

namespace VoxelEngine
{

Structure::Structure(VoxelType rootType) : m_Radius(0), m_VoxelData({}), m_Root(rootType)
{
}

Voxel& Structure::GetRoot()
{
    return m_Root;
}

int32_t Structure::GetRadius() const
{
    return m_Radius;
}

const std::vector<std::pair<glm::i16vec3, VoxelType> >& Structure::GetVoxelData()
{
    return m_VoxelData;
}

void Structure::SetRootPosition(Position3D p)
{
    m_Root.SetPosition(p);
}

}
