//
// Created by RadU on 2/17/2026.
//

#include "AnimationComponent.hpp"

#include "imgui.h"

namespace VoxelEngine
{

std::string AnimationComponent::GetName() { return "AnimationComponent"; }

void AnimationComponent::DrawGui()
{
    if (!ImGui::BeginTable("Animation Table", 2, ImGuiTableFlags_SizingStretchSame))
        return;

    for (auto& [Name, IsActive, Time, Duration, NodeAnimations]: Animations)
    {
        ImGui::TableNextRow();

        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted("Animation");

        ImGui::TableSetColumnIndex(1);
        ImGui::SetNextItemWidth(-FLT_MIN);

        if (ImGui::Button(IsActive ? "Deactivate" : "Activate"))
            IsActive = !IsActive;
    }

    ImGui::EndTable();
}

}
