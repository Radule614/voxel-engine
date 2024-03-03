#pragma once

#include <vector>
#include <map>
#include <memory>

#include "Vertex.hpp"
#include "Voxel.hpp"

#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>

#define CHUNK_WIDTH 16
#define CHUNK_HEIGHT 64

namespace Terrain
{
class Chunk
{
public:
    Chunk();
    Chunk(glm::vec3 position);
    ~Chunk();

    struct VoxelPosition
    {
        glm::vec3 Vector;

        bool operator<(const VoxelPosition &pos) const noexcept
        {
            if (this->Vector.y != pos.Vector.y)
                return this->Vector.y < pos.Vector.y;
            if (this->Vector.z != pos.Vector.z)
                return this->Vector.z < pos.Vector.z;
            return this->Vector.x < pos.Vector.x;
        }

        VoxelPosition(glm::vec3 pos) : Vector(pos)
        {
        }
    };

    inline std::vector<Vertex> GetMesh() const
    {
        return m_Mesh;
    }

    inline glm::vec3 GetPosition() const
    {
        return m_Position;
    }

    glm::mat4 GetModelMatrix() const
    {
        glm::mat4 model(1.0f);
        glm::vec3 pos = m_Position;
        pos.x *= CHUNK_WIDTH;
        pos.y *= CHUNK_HEIGHT;
        pos.z *= CHUNK_WIDTH;
        return glm::translate(model, pos);
    }

private:
    void Init();
    void GenerateMesh(std::vector<std::vector<std::vector<Voxel>>> &voxelGrid);

private:
    glm::vec3 m_Position;
    std::vector<Vertex> m_Mesh;
    std::map<VoxelPosition, Voxel> m_VoxelMap;
};
}; // namespace Terrain
