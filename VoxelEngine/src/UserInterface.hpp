#pragma once

#include "EngineState.hpp"
#include "entt.hpp"

namespace VoxelEngine
{

class UserInterface : public GLCore::Layer
{
public:
    explicit UserInterface(EngineState& state);
    ~UserInterface() override;

    void OnAttach() override;
    void OnEvent(GLCore::Event& event) override;
    void OnImGuiRender() override;
    void OnUpdate(GLCore::Timestep ts) override;

private:
    void DrawSceneViewer();
    void DrawComponentViewer() const;
    void DrawNode(entt::entity entity);

private:
    EngineState& m_State;

    entt::entity m_SelectedEntity = entt::null;
};
}
