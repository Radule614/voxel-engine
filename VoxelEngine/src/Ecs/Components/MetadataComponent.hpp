//
// Created by RadU on 2/4/2026.
//

#pragma once

#include <string>

namespace VoxelEngine
{

struct MetadataComponent
{
    std::string Name;

    explicit MetadataComponent(const std::string& name) : Name(name)
    {
    }
};

}
