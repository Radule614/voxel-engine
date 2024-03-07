#pragma once

#include <vector>

#include <glm/glm.hpp>

namespace Terrain
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
    Voxel(VoxelType type, glm::vec3 position);

    bool IsFaceVisible(VoxelFace face) const;
    void SetFaceVisible(VoxelFace face, bool visible);
    void SetAllFacesVisible(bool visible);

    inline void SetVoxelType(VoxelType type)
    {
        m_VoxelType = type;
    }

    inline VoxelType GetVoxelType() const
    {
        return m_VoxelType;
    }
    inline void SetPosition(glm::vec3 pos)
    {
        m_Position = pos;
    }

    inline glm::vec3 GetPosition() const
    {
        return m_Position;
    }

    static VoxelFace GetOpositeFace(VoxelFace face);

private:
    VoxelType m_VoxelType;
    bool m_VisibleFaces[6];
    glm::vec3 m_Position;
};
}; // namespace Terrain
