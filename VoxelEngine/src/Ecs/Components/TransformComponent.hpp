#pragma once

#include <glm/glm.hpp>
#include "glm/detail/type_quat.hpp"

namespace VoxelEngine
{

struct TransformComponent
{
    glm::vec3 LocalPosition{0};
    glm::quat LocalRotation{1, 0, 0, 0};
    glm::vec3 LocalScale{1};

    glm::mat4 LocalMatrix{1};
    glm::mat4 WorldMatrix{1};

    glm::vec3 WorldPosition{0};
    glm::vec3 PreviousWorldPosition{0};

    bool IsDirty = true;
};

}
