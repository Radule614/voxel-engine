#pragma once

#include "GLCore/Core/Layer.hpp"
#include "GLCore/Events/ApplicationEvent.hpp"
#include "GLCore/Events/KeyEvent.hpp"
#include "GLCore/Events/MouseEvent.hpp"

namespace GLCore
{

class ImGuiLayer : public Layer
{
public:
	ImGuiLayer();
	~ImGuiLayer() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	void Begin();
	void End();

	virtual void OnEvent(Event& event);
	bool OnMouseButtonPressed(MouseButtonPressedEvent& e);
private:
	float m_Time = 0.0f;
};

}