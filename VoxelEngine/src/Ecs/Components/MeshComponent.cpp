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

}
