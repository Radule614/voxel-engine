#include "Voxel.hpp"

#include <glm/glm.hpp>

namespace VoxelEngine
{

Voxel::Voxel() : Voxel(AIR, Position3D())
{
}

Voxel::~Voxel() = default;

Voxel::Voxel(VoxelType type) : Voxel(type, Position3D())
{
}

Voxel::Voxel(const VoxelType type, const Position3D position) : m_Position(position),
                                                                m_VoxelType(type),
                                                                m_VisibleFaces(0)
{
}

void Voxel::SetFaceVisible(VoxelFace face, bool visible)
{
    if (visible)
        m_VisibleFaces |= (1 << static_cast<uint8_t>(face));
    else
        m_VisibleFaces &= ~(1 << static_cast<uint8_t>(face));
}

bool Voxel::IsFaceVisible(VoxelFace face) const { return m_VisibleFaces & (1 << static_cast<uint8_t>(face)); }

void Voxel::SetAllFacesVisible(bool visible) { m_VisibleFaces = visible ? 0x3F : 0; }

VoxelFace Voxel::GetOppositeFace(VoxelFace face)
{
    switch (face)
    {
        case TOP:
            return BOTTOM;
        case BOTTOM:
            return TOP;
        case FRONT:
            return BACK;
        case BACK:
            return FRONT;
        case RIGHT:
            return LEFT;
        case LEFT:
            return RIGHT;
    }
    return face;
}

void Voxel::SetVoxelType(VoxelType type) { m_VoxelType = type; }

VoxelType Voxel::GetVoxelType() const { return static_cast<VoxelType>(m_VoxelType); }

void Voxel::SetPosition(Position3D pos) { m_Position = pos; }

Position3D Voxel::GetPosition() const { return m_Position; }

bool Voxel::IsTransparent() const { return m_VoxelType == AIR || m_VoxelType == LEAVES; }

};
