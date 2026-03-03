//
// Created by RadU on 2/15/2026.
//

#pragma once

#include "../../Utils/Utils.hpp"
#include "glm/glm.hpp"

namespace VoxelEngine::RenderUtils
{

void DrawQuad();
void DrawCube(const GLCore::Utils::Shader& shader, const glm::mat4& model);
void DrawSphere(const GLCore::Utils::Shader& shader, const glm::mat4& model);

}
