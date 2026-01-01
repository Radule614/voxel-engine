#pragma once

#include <glm/glm.hpp>
#include "../../Utils/Utils.hpp"
#include <string>

namespace VoxelEngine
{

struct Position3D
{
    Position3D();
    Position3D(uint8_t x, uint8_t y, uint8_t z);
    explicit Position3D(glm::i32vec3 v);

    uint8_t GetX() const;
    uint8_t GetY() const;
    uint8_t GetZ() const;

    void SetX(uint8_t x);
    void SetY(uint8_t y);
    void SetZ(uint8_t z);

    bool operator<(const Position3D& pos) const noexcept;
    bool operator==(const Position3D& pos) const noexcept;
    Position3D operator+(Position3D const& other) const;
    explicit operator glm::ivec3() const;
    explicit operator glm::vec3() const;

    std::string ToString() const;

private:
    uint8_t xz; // 4 bits for x and 4 bits for z
    uint8_t y;
};

};

template<>
struct std::hash<VoxelEngine::Position3D>
{
    std::size_t operator()(const VoxelEngine::Position3D& p) const noexcept;
};;
