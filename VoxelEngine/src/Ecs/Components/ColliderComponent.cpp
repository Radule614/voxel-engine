//
// Created by RadU on 2/17/2026.
//

#include "ColliderComponent.hpp"

#include "magic_enum.hpp"

namespace VoxelEngine
{

using namespace JPH;

ColliderComponent::ColliderComponent(const BodyID bodyId, const EShapeType type, const EShapeSubType subType)
    : BodyId(bodyId), ShapeType(type), ShapeSubType(subType)
{
}

std::string ColliderComponent::GetName() { return "Collider Component"; }

void ColliderComponent::DrawGui()
{
    if (!ImGui::BeginTable("ColliderTable", 2, ImGuiTableFlags_SizingStretchSame))
        return;

    ImGui::TableNextRow();

    ImGui::TableSetColumnIndex(0);
    ImGui::TextUnformatted("Shape Type");

    ImGui::TableSetColumnIndex(1);
    ImGui::SetNextItemWidth(-FLT_MIN);
    ImGui::Text("%s",  magic_enum::enum_name(ShapeType).data());

    ImGui::TableNextRow();

    ImGui::TableSetColumnIndex(0);
    ImGui::TextUnformatted("Shape Sub Type");

    ImGui::TableSetColumnIndex(1);
    ImGui::SetNextItemWidth(-FLT_MIN);
    ImGui::Text("%s", magic_enum::enum_name(ShapeSubType).data());

    ImGui::EndTable();
}

}
