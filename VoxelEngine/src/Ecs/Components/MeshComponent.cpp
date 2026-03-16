//
// Created by RadU on 2/17/2026.
//

#include "MeshComponent.hpp"

#include "imgui.h"

namespace VoxelEngine
{

MeshComponent::MeshComponent(std::string name, const std::vector<RenderPrimitive>& primitives)
    : Name(std::move(name)), Primitives(primitives)
{
}

std::string MeshComponent::GetName() { return "Mesh Component"; }

void MeshComponent::DrawGui()
{
    if (!ImGui::BeginTable("Mesh Table", 2, ImGuiTableFlags_SizingStretchSame))
        return;

    ImGui::TableNextRow();

    ImGui::TableSetColumnIndex(0);
    ImGui::TextUnformatted("Mesh");

    ImGui::TableSetColumnIndex(1);
    ImGui::SetNextItemWidth(-FLT_MIN);
    ImGui::Text("%s", Name.c_str());

    ImGui::EndTable();
}

MeshComponent MeshComponent::CreateQuadMesh()
{
    static constexpr float vertices[] = {
        -0.5f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
         0.5f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
         0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f,
    };
    static constexpr unsigned int indices[] = { 0, 1, 2,  1, 3, 2 };

    GLuint vao, vbo, ebo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    constexpr GLsizei stride = 8 * sizeof(float);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));

    glBindVertexArray(0);

    RenderPrimitive primitive;
    primitive.Vao         = vao;
    primitive.Mode        = GL_TRIANGLES;
    primitive.IndexCount  = 6;
    primitive.IndexType   = GL_UNSIGNED_INT;
    primitive.IndexOffset = nullptr;

    return MeshComponent("Quad", { primitive });
}

}
