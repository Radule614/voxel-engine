#include "Voxel.hpp"

Voxel::Voxel() : m_VoxelType(VoxelType::AIR), m_VisibleFaces(6, false)
{
}

Voxel::~Voxel()
{
}

void Voxel::SetFaceVisible(VoxelFace face, bool visible)
{
    m_VisibleFaces[face] = visible;
}

bool Voxel::IsFaceVisible(VoxelFace face)
{
    return m_VisibleFaces[face];
}
