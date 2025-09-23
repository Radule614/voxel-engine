#pragma once

#include <glm/glm.hpp>
#include "VoxelConstants.hpp"
#include "Position3D.hpp"

namespace VoxelEngine
{

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

    static VoxelFace GetOppositeFace(VoxelFace face);
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
