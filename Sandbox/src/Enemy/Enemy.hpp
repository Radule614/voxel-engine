//
// Created by RadU on 11/1/2025.
//

#pragma once

#include <memory>

#include <Physics/Character/CharacterBuilder.hpp>

#include "entt.hpp"

namespace Sandbox
{

class Enemy
{
public:
    Enemy(glm::vec3 position);
    ~Enemy();

private:
    std::unique_ptr<JPH::Character> m_Character;
    entt::entity m_Entity;
};

}
