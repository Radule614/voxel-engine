#include "Voxel.hpp"

#include <glm/glm.hpp>

namespace Terrain
{
Voxel::Voxel() : Voxel(VoxelType::AIR, glm::vec3(0))
{
}

Voxel::~Voxel()
{
}

Voxel::Voxel(VoxelType type) : Voxel(type, glm::vec3(0))
{
}

Voxel::Voxel(VoxelType type, glm::vec3 position) : m_VoxelType(type), m_Position(position), m_VisibleFaces(6, false)
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
        m_VisibleFaces[i] = visible;
}
}; // namespace Terrain
