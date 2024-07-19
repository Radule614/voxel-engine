#include "Voxel.hpp"

#include <glm/glm.hpp>

namespace VoxelEngine
{

Voxel::Voxel() : Voxel(VoxelType::AIR, Position3D())
{
}

Voxel::~Voxel()
{
}

Voxel::Voxel(VoxelType type) : Voxel(type, Position3D())
{
}

Voxel::Voxel(VoxelType type, Position3D position) : m_VoxelType(type), m_Position(position), m_VisibleFaces(0)
{
}

void Voxel::SetFaceVisible(VoxelFace face, bool visible)
{
	if (visible)
		m_VisibleFaces |= (1 << static_cast<uint8_t>(face));
	else
		m_VisibleFaces &= ~(1 << static_cast<uint8_t>(face));
}

bool Voxel::IsFaceVisible(VoxelFace face) const
{
	return m_VisibleFaces & (1 << static_cast<uint8_t>(face));
}

void Voxel::SetAllFacesVisible(bool visible)
{
	m_VisibleFaces = visible ? 0x3F : 0;
}

VoxelFace Voxel::GetOpositeFace(VoxelFace face)
{
	switch (face)
	{
	case VoxelFace::TOP:
		return VoxelFace::BOTTOM;
	case VoxelFace::BOTTOM:
		return VoxelFace::TOP;
	case VoxelFace::FRONT:
		return VoxelFace::BACK;
	case VoxelFace::BACK:
		return VoxelFace::FRONT;
	case VoxelFace::RIGHT:
		return VoxelFace::LEFT;
	case VoxelFace::LEFT:
		return VoxelFace::RIGHT;
	}
	return face;
}

void Voxel::SetVoxelType(VoxelType type)
{
	m_VoxelType = type;
}

VoxelType Voxel::GetVoxelType() const
{
	return (VoxelType)m_VoxelType;
}

void Voxel::SetPosition(Position3D pos)
{
	m_Position = pos;
}

Position3D Voxel::GetPosition() const
{
	return m_Position;
}

bool Voxel::IsTransparent() const
{
	return m_VoxelType == VoxelType::AIR || m_VoxelType == VoxelType::LEAVES;
}

};
