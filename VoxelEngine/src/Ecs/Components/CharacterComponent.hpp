//
// Created by RadU on 14-Jun-25.
//

#pragma once

#include "../../Physics/Character/CharacterController.hpp"

namespace VoxelEngine
{

struct CharacterComponent
{
    std::unique_ptr<CharacterController> Controller;
    glm::vec3 Velocity = glm::vec3(0.0f);

    explicit CharacterComponent(std::unique_ptr<CharacterController>&& character)
        : Controller(std::move(character))
    {
    }
};

}
