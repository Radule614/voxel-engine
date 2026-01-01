//
// Created by RadU on 10/12/2025.
//

#pragma once

namespace VoxelEngine
{

enum BiomeType
{
    UNKNOWN = 0,
    PLAINS,
    DESERT,
    SNOWY_PLAINS
};

}

namespace std
{

template<>
struct hash<VoxelEngine::BiomeType>
{
    std::size_t operator()(const VoxelEngine::BiomeType biomeType) const noexcept
    {
        return static_cast<std::size_t>(biomeType);
    }
};

}
