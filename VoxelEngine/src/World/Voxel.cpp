#include "Voxel.hpp"

Voxel::Voxel() : m_VoxelType(VoxelType::AIR), m_VisibleFaces(6, false)
{
}

Voxel::~Voxel()
{
}

Voxel::Voxel(VoxelType type) : m_VoxelType(type), m_VisibleFaces(6, false)
{
}

void Voxel::SetFaceVisible(VoxelFace face, bool visible)
{
    m_VisibleFaces[face] = visible;
}

bool Voxel::IsFaceVisible(VoxelFace face) const
{
    return m_VisibleFaces[face];
}

void Voxel::SetAllFacesVisible(bool visible)
{
    for (size_t i = 0; i < m_VisibleFaces.size(); i++)
    {
        m_VisibleFaces[i] = visible;
    }
}
