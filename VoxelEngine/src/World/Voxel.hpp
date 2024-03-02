#pragma once

#include <vector>

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

    bool IsFaceVisible(VoxelFace face) const;
    void SetFaceVisible(VoxelFace face, bool visible);
    void SetAllFacesVisible(bool visible);
    inline VoxelType GetVoxelType() const
    {
        return m_VoxelType;
    }

private:
    VoxelType m_VoxelType;
    std::vector<bool> m_VisibleFaces;
};
