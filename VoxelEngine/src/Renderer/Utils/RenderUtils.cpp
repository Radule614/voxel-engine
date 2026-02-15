//
// Created by RadU on 2/15/2026.
//

#include "RenderUtils.hpp"
#include "../Renderer.hpp"
#include "../../Assets/AssetManager.hpp"

#include "glad/glad.h"

using namespace GLCore;
using namespace GLCore::Utils;

namespace VoxelEngine
{

void RenderUtils::DrawQuad()
{
    static unsigned int vao = 0;
    static unsigned int vbo;

    if (vao == 0)
    {
        constexpr float vertices[] = {
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float_t), (void*)0);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float_t), (void*)(3 * sizeof(float_t)));
    }

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void RenderUtils::DrawCube(const Shader& shader, const glm::mat4& model)
{
    Renderer::DrawPrimitives(shader, AssetManager::Instance().GetCubeModel().GetMeshPrimitives(0), model);
}

void RenderUtils::DrawSphere(const Shader& shader, const glm::mat4& model)
{
    Renderer::DrawPrimitives(shader, AssetManager::Instance().GetSphereModel().GetMeshPrimitives(0), model);
}

}
