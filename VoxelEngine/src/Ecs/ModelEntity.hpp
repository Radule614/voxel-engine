//
// Created by RadU on 2/15/2026.
//

#pragma once

#include "Ecs.hpp"

#include "../Assets/Gltf/Model.hpp"

namespace VoxelEngine
{

entt::entity CreateEntityFromModel(const Model& model);

}
