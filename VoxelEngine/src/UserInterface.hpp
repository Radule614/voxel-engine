#pragma once

#include "EngineState.hpp"

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
    EngineState& m_State;
};
}
