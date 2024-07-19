#pragma once

#include <vector>

#include <glm/glm.hpp>
#include "Position3D.hpp"

namespace VoxelEngine
{

enum VoxelType
{
	AIR = 0,
	DIRT,
	GRASS,
	STONE,
	LOG,
	LEAVES,
	SNOW,
	DIRT_SNOW
};

enum VoxelFace
{
	TOP = 0,
	BOTTOM,
	FRONT,
	RIGHT,
	BACK,
	LEFT
};

class Voxel
{
public:
	Voxel();
	~Voxel();
	Voxel(VoxelType type);
	Voxel(VoxelType type, Position3D position);

	bool IsFaceVisible(VoxelFace face) const;
	void SetFaceVisible(VoxelFace face, bool visible);
	void SetAllFacesVisible(bool visible);

	static VoxelFace GetOpositeFace(VoxelFace face);
	void SetVoxelType(VoxelType type);
	VoxelType GetVoxelType() const;
	void SetPosition(Position3D pos);
	Position3D GetPosition() const;
	bool IsTransparent() const;

private:
	Position3D m_Position;
	uint8_t m_VoxelType;
	uint8_t m_VisibleFaces;
};

};
