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

    bool IsFaceVisible(VoxelFace face);
    void SetFaceVisible(VoxelFace face, bool visible);

private:
    VoxelType m_VoxelType;
    std::vector<bool> m_VisibleFaces;
};
