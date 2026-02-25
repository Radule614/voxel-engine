//
// Created by RadU on 2/23/2026.
//

#include "SkinComponent.hpp"

#include "imgui.h"

namespace VoxelEngine
{

std::string SkinComponent::GetName() { return "SkinComponent"; }

void SkinComponent::DrawGui()
{
    if (!ImGui::BeginTable("Skin Table", 2, ImGuiTableFlags_SizingStretchSame))
        return;

    ImGui::TableNextRow();

    ImGui::TableSetColumnIndex(0);
    ImGui::TextUnformatted("Bone Count");

    ImGui::TableSetColumnIndex(1);
    ImGui::SetNextItemWidth(-FLT_MIN);
    ImGui::Text("%lld", JointEntities.size());

    ImGui::TableNextRow();

    ImGui::TableSetColumnIndex(0);
    ImGui::TextUnformatted("Skinning");

    ImGui::TableSetColumnIndex(1);
    ImGui::SetNextItemWidth(-FLT_MIN);
    if (ImGui::Button(IsEnabled ? "Disable" : "Enable"))
        IsEnabled = !IsEnabled;

    ImGui::EndTable();
}

}
