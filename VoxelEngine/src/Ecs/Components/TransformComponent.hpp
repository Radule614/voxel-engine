#pragma once

#include <glm/glm.hpp>

#include "../ComponentGui.hpp"
#include "glm/detail/type_quat.hpp"

namespace VoxelEngine
{

struct TransformComponent : ComponentGui
{
    glm::vec3 LocalPosition{0};
    glm::quat LocalRotation{1, 0, 0, 0};
    glm::vec3 LocalScale{1};

    glm::mat4 LocalMatrix{1};
    glm::mat4 WorldMatrix{1};

    glm::vec3 WorldPosition{0};
    glm::vec3 PreviousWorldPosition{0};

    bool IsDirty = true;

    TransformComponent() = default;
    ~TransformComponent() override = default;

    std::string GetName() override;
    void DrawGui() override;
};

}
