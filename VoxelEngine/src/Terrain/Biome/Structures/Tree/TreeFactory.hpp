//
// Created by RadU on 9/27/2025.
//

#pragma once

#include "../Structure.hpp"

namespace VoxelEngine
{

class TreeFactory {
public:
    static Structure CreateTree(Position3D position);
    static Structure CreateLargeTree(Position3D position);
};

}

