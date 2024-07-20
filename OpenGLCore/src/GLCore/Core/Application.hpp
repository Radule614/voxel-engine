#pragma once

#include "Core.hpp"
#include "Window.hpp"
#include "LayerStack.hpp"
#include "../Events/Event.hpp"
#include "../Events/ApplicationEvent.hpp"
#include "../Events/KeyEvent.hpp"
#include "Timestep.hpp"
#include "../ImGui/ImGuiLayer.hpp"
#include "KeyCodes.hpp"
namespace GLCore
{
class Application
{
public:
	Application(const std::string& name = "Voxel Engine", uint32_t width = 1920, uint32_t height = 1080);
	virtual ~Application() = default;

	void Run();

	void OnEvent(Event& e);
	void PushLayer(Layer* layer);
	void PushOverlay(Layer* layer);

	inline Window& GetWindow()
	{
		return *m_Window;
	}
	inline static Application& Get()
	{
		return *s_Instance;
	}

	inline void Stop()
	{
		m_Running = false;
	}

private:
	bool OnWindowClose(WindowCloseEvent& e);

private:
	std::unique_ptr<Window> m_Window;
	bool m_Running = true;
	LayerStack m_LayerStack;
	float m_LastFrameTime = 0.0f;
	ImGuiLayer* m_ImGuiLayer;
	static Application* s_Instance;
};
} // namespace GLCore
