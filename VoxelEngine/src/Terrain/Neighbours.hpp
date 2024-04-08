#pragma once

#include <memory>

namespace Terrain
{
class Chunk;

struct Neighbours
{
    std::shared_ptr<Chunk> front = nullptr;
    std::shared_ptr<Chunk> back = nullptr;
    std::shared_ptr<Chunk> right = nullptr;
    std::shared_ptr<Chunk> left = nullptr;
};
} // namespace Terrain
