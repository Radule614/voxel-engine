//
// Created by RadU on 3/1/2026.
//

#pragma once

#include <utility>
#include "entt.hpp"
#include "../ComponentGui.hpp"
#include "GLCore/Core/Timestep.hpp"
#include "GLCore/Events/Event.hpp"

namespace VoxelEngine
{

struct ScriptContext
{
    entt::registry& Registry;
    entt::entity Entity;
};

class Script
{
public:
    explicit Script(std::string name) : m_Name(std::move(name))
    {
    }

    virtual ~Script() = default;

    std::string GetName() const { return m_Name; }

    virtual void OnUpdate(GLCore::Timestep ts, ScriptContext context) = 0;
    virtual void OnEvent(GLCore::Event& event, ScriptContext context) = 0;

private:
    const std::string m_Name;
};

struct ScriptComponent : ComponentGui
{
    ScriptComponent() = default;
    ~ScriptComponent() override = default;

    ScriptComponent(const ScriptComponent&) = delete;
    ScriptComponent& operator=(const ScriptComponent&) = delete;
    ScriptComponent(ScriptComponent&&) noexcept = default;
    ScriptComponent& operator=(ScriptComponent&&) noexcept = default;

    std::vector<std::unique_ptr<Script> > Scripts{};

    std::string GetName() override;
    void DrawGui() override;
};

}
