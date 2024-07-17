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
	SNOW,
	STONE
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

private:
	VoxelType m_VoxelType;
	bool m_VisibleFaces[6];
	Position3D m_Position;
};

};
