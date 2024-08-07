#pragma once

#include "GLCore.hpp"
#include "GLCoreUtils.hpp"
#include "EngineState.hpp"

namespace VoxelEngine
{

class UserInterface : public GLCore::Layer
{
public:
	UserInterface(EngineState& state, GLCore::Application& app);
	~UserInterface();

	virtual void OnAttach() override;
	virtual void OnEvent(GLCore::Event& event) override;
	virtual void OnImGuiRender() override;

private:
	GLCore::Application& m_Application;
	EngineState& m_State;
};
}