//
// Created by RadU on 2/17/2026.
//

#include "TransformComponent.hpp"

#include "imgui.h"

namespace VoxelEngine
{

std::string TransformComponent::GetName() { return "Transform Component"; }

void TransformComponent::DrawGui()
{
    if (!ImGui::BeginTable("TransformTable", 2, ImGuiTableFlags_SizingStretchSame))
        return;

    ImGui::TableNextRow();

    ImGui::TableSetColumnIndex(0);
    ImGui::TextUnformatted("Position");

    ImGui::TableSetColumnIndex(1);
    ImGui::SetNextItemWidth(-FLT_MIN);
    if (ImGui::DragFloat3("TransformPosition", &LocalPosition.x, 0.1f))
        IsDirty = true;

    ImGui::TableNextRow();

    ImGui::TableSetColumnIndex(0);
    ImGui::TextUnformatted("Rotation");

    ImGui::TableSetColumnIndex(1);
    ImGui::SetNextItemWidth(-FLT_MIN);
    if (ImGui::DragFloat4("TransformRotation", &LocalRotation.x, 0.1f))
        IsDirty = true;

    ImGui::TableNextRow();

    ImGui::TableSetColumnIndex(0);
    ImGui::TextUnformatted("Scale");

    ImGui::TableSetColumnIndex(1);
    ImGui::SetNextItemWidth(-FLT_MIN);
    if (ImGui::DragFloat3("TransformScale", &LocalScale.x, 0.1f))
        IsDirty = true;

    ImGui::EndTable();
}

}
