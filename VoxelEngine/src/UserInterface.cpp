#include "UserInterface.hpp"

#include "GLCore/Core/Input.hpp"

using namespace GLCore;
using namespace GLCore::Utils;

namespace VoxelEngine
{

UserInterface::UserInterface(EngineState& state, GLCore::Application& app) : m_State(state), m_Application(app)
{
}

UserInterface::~UserInterface()
{
}

void UserInterface::OnAttach()
{
	m_Application.GetWindow().CaptureMouse(true);
}

void UserInterface::OnEvent(GLCore::Event& event)
{
	EventDispatcher dispatcher(event);
	dispatcher.Dispatch<KeyPressedEvent>(
		[&](KeyPressedEvent& e)
		{
			if (e.GetKeyCode() == HZ_KEY_ESCAPE)
			{
				m_State.MenuActive = !m_State.MenuActive;
				if (!m_State.MenuActive)
				{
					m_Application.GetWindow().CaptureMouse(true);
					StateUnpauseEvent event;
					m_Application.RaiseStateEvent(event);
				}
				else
				{
					m_Application.GetWindow().CaptureMouse(false);
					StatePauseEvent event;
					m_Application.RaiseStateEvent(event);
				}
			}
			return true;
		});
}

void UserInterface::OnImGuiRender()
{
	if (!m_State.MenuActive)
		return;

	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove;
	auto& io = ImGui::GetIO();
	ImGui::SetNextWindowSize(ImVec2(300.0, 400.0));
	ImGui::SetNextWindowPos(ImVec2(0, 0));

	ImGui::Begin("Test", nullptr, windowFlags);
	ImGui::Text("Hello, World!");
	if (ImGui::Button("Click Me"))
		ImGui::Text("Button was clicked!");
	ImGui::End();
}

}
