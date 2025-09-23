#pragma once

#include "../Voxel.hpp"
#include <vector>

namespace VoxelEngine
{

class Structure
{
public:
    const Voxel& GetRoot() const;
    int32_t GetRadius() const;
    const std::vector<std::pair<glm::i32vec3, VoxelType> >& GetVoxelData() const;
    void SetRootPosition(Position3D position);

protected:
    explicit Structure(VoxelType rootType);

protected:
    Voxel m_Root;
    int32_t m_Radius;
    std::vector<std::pair<glm::i32vec3, VoxelType> > m_VoxelData;
};

}
