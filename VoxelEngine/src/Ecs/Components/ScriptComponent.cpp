//
// Created by RadU on 3/1/2026.
//

#include "ScriptComponent.hpp"

#include "imgui.h"

namespace VoxelEngine
{

std::string ScriptComponent::GetName() { return "ScriptComponent"; }

void ScriptComponent::DrawGui()
{
    if (!ImGui::BeginTable("Script Table", 2, ImGuiTableFlags_SizingStretchSame))
        return;

    int i = 0;
    for (const auto& [Name, OnUpdate]: Scripts)
    {
        ImGui::PushID(i++);

        ImGui::TableNextRow();

        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted(Name.c_str());

        ImGui::PopID();
    }

    ImGui::EndTable();
}

}
