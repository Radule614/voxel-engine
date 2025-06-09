#pragma once
#include "GLCore.hpp"
#include <GLCoreUtils.hpp>
#include "../EngineState.hpp"

namespace VoxelEngine
{

class EcsLayer : public GLCore::Layer
{
public:
    EcsLayer(EngineState& state);
    ~EcsLayer();

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnUpdate(GLCore::Timestep ts) override;

private:
    EngineState& m_State;
};

}
