#pragma once

#include "GLCore.hpp"
#include "GLCoreUtils.hpp"
#include "../EngineState.hpp"

namespace VoxelEngine
{

class PhysicsLayer : public GLCore::Layer
{
public:
	PhysicsLayer(EngineState& state);
	~PhysicsLayer();

	virtual void OnAttach() override;
	virtual void OnUpdate(GLCore::Timestep ts) override;
private:
	EngineState& m_State;
};

}
